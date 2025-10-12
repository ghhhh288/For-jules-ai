"""
Main Application - Face Scanning with LLM Integration
Real-time face detection, analysis, and AI-powered insights
"""

import cv2
import numpy as np
from typing import Optional, Dict
import logging
import argparse
from pathlib import Path
import json
from datetime import datetime

from face_scanner import FaceScanner
from llm_engine import LLMEngine

logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)


class FaceScanApp:
    """Main application for face scanning with LLM integration"""
    
    def __init__(self, llm_provider: str = "openai", llm_model: str = None, 
                 detection_model: str = "hog"):
        """
        Initialize Face Scan Application
        
        Args:
            llm_provider: LLM provider ('openai' or 'anthropic')
            llm_model: LLM model name (optional)
            detection_model: Face detection model ('hog' or 'cnn')
        """
        self.scanner = FaceScanner(model=detection_model)
        try:
            self.llm = LLMEngine(provider=llm_provider, model=llm_model)
            self.llm_available = True
        except Exception as e:
            logger.warning(f"LLM not available: {e}")
            logger.warning("Running in face detection only mode")
            self.llm = None
            self.llm_available = False
        
        self.current_scan_result = None
        self.camera = None
        logger.info("Face Scan Application initialized")
    
    def start_camera(self, camera_id: int = 0) -> bool:
        """
        Start camera capture
        
        Args:
            camera_id: Camera device ID
            
        Returns:
            True if successful
        """
        try:
            self.camera = cv2.VideoCapture(camera_id)
            if not self.camera.isOpened():
                logger.error(f"Could not open camera {camera_id}")
                return False
            logger.info(f"Camera {camera_id} opened successfully")
            return True
        except Exception as e:
            logger.error(f"Error starting camera: {e}")
            return False
    
    def stop_camera(self):
        """Stop camera capture"""
        if self.camera:
            self.camera.release()
            logger.info("Camera released")
    
    def process_frame(self, frame: np.ndarray, 
                     analyze_features: bool = True) -> tuple[np.ndarray, Dict]:
        """
        Process single frame with face detection and analysis
        
        Args:
            frame: Input frame from camera
            analyze_features: Whether to perform deep feature analysis
            
        Returns:
            Tuple of (annotated_frame, scan_results)
        """
        if analyze_features:
            scan_results = self.scanner.comprehensive_scan(frame)
        else:
            # Quick detection only
            face_locations = self.scanner.detect_faces(frame)
            scan_results = {
                'timestamp': datetime.now().isoformat(),
                'total_faces': len(face_locations),
                'faces': [{'location': {
                    'top': loc[0], 'right': loc[1], 
                    'bottom': loc[2], 'left': loc[3]
                }} for loc in face_locations]
            }
        
        annotated_frame = self.scanner.draw_annotations(frame, scan_results)
        return annotated_frame, scan_results
    
    def run_camera_mode(self, analyze_interval: int = 30, 
                       show_video: bool = True):
        """
        Run real-time camera face scanning
        
        Args:
            analyze_interval: Frames between full analysis (lower = slower but more accurate)
            show_video: Whether to display video window
        """
        if not self.start_camera():
            logger.error("Failed to start camera")
            return
        
        frame_count = 0
        
        try:
            print("\n=== Face Scan Camera Mode ===")
            print("Press 'q' to quit")
            print("Press 's' to save current analysis")
            print("Press 'a' to get AI analysis of current frame")
            print("Press 'r' to register face")
            print("Press 'c' to chat with AI")
            print("=" * 40 + "\n")
            
            while True:
                ret, frame = self.camera.read()
                if not ret:
                    logger.error("Failed to read frame")
                    break
                
                # Perform analysis at intervals
                analyze_now = (frame_count % analyze_interval == 0)
                annotated_frame, scan_results = self.process_frame(
                    frame, analyze_features=analyze_now
                )
                
                if analyze_now:
                    self.current_scan_result = scan_results
                    self._print_scan_summary(scan_results)
                
                # Display video
                if show_video:
                    cv2.imshow('Face Scan - Press q to quit', annotated_frame)
                
                # Handle keyboard input
                key = cv2.waitKey(1) & 0xFF
                
                if key == ord('q'):
                    print("\nQuitting...")
                    break
                elif key == ord('s'):
                    self._save_analysis(frame, scan_results)
                elif key == ord('a'):
                    self._get_ai_analysis(scan_results)
                elif key == ord('r'):
                    self._register_face_interactive(frame)
                elif key == ord('c'):
                    self._chat_interactive(scan_results)
                
                frame_count += 1
        
        except KeyboardInterrupt:
            print("\nInterrupted by user")
        finally:
            self.stop_camera()
            cv2.destroyAllWindows()
    
    def analyze_image(self, image_path: str) -> Dict:
        """
        Analyze single image file
        
        Args:
            image_path: Path to image file
            
        Returns:
            Analysis results
        """
        try:
            image = cv2.imread(image_path)
            if image is None:
                logger.error(f"Could not read image: {image_path}")
                return {}
            
            scan_results = self.scanner.comprehensive_scan(image)
            
            # Generate AI analysis if available
            if self.llm_available:
                ai_analysis = self.llm.analyze_face_data(scan_results)
                scan_results['ai_analysis'] = ai_analysis
            
            # Save annotated image
            annotated = self.scanner.draw_annotations(image, scan_results)
            output_path = Path(image_path).stem + "_analyzed.jpg"
            cv2.imwrite(output_path, annotated)
            logger.info(f"Annotated image saved: {output_path}")
            
            return scan_results
            
        except Exception as e:
            logger.error(f"Error analyzing image: {e}")
            return {}
    
    def _print_scan_summary(self, scan_results: Dict):
        """Print summary of scan results"""
        total = scan_results.get('total_faces', 0)
        if total > 0:
            print(f"\n[{scan_results.get('timestamp')}] Detected {total} face(s)")
            for i, face in enumerate(scan_results.get('faces', []), 1):
                features = face.get('features', {})
                print(f"  Face {i}: {face.get('name', 'Unknown')} - "
                      f"Age: {features.get('age', 'N/A')}, "
                      f"Gender: {features.get('gender', 'N/A')}, "
                      f"Emotion: {features.get('emotion', 'N/A')}")
    
    def _save_analysis(self, frame: np.ndarray, scan_results: Dict):
        """Save current analysis to file"""
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        
        # Save image
        image_path = f"scan_{timestamp}.jpg"
        annotated = self.scanner.draw_annotations(frame, scan_results)
        cv2.imwrite(image_path, annotated)
        
        # Save data
        data_path = f"scan_{timestamp}.json"
        with open(data_path, 'w') as f:
            json.dump(scan_results, f, indent=2)
        
        print(f"\n✓ Analysis saved: {image_path}, {data_path}")
    
    def _get_ai_analysis(self, scan_results: Dict):
        """Get AI analysis of current scan"""
        if not self.llm_available:
            print("\n✗ LLM not available. Configure API key to enable AI analysis.")
            return
        
        print("\n🤖 Generating AI analysis...")
        try:
            analysis = self.llm.analyze_face_data(scan_results)
            print("\n" + "=" * 60)
            print("AI ANALYSIS:")
            print("=" * 60)
            print(analysis)
            print("=" * 60 + "\n")
        except Exception as e:
            print(f"\n✗ Error generating analysis: {e}")
    
    def _register_face_interactive(self, frame: np.ndarray):
        """Register a face interactively"""
        print("\nEnter name for face registration (or 'cancel'): ", end='', flush=True)
        
        # Note: In real terminal this would work, but may not work in all environments
        # For production, consider using GUI input
        try:
            import sys
            import select
            
            # Simple input with timeout
            name = input().strip()
            
            if name.lower() == 'cancel':
                print("Registration cancelled")
                return
            
            if self.scanner.register_face(frame, name):
                print(f"✓ Face registered for: {name}")
            else:
                print("✗ Failed to register face")
        except Exception as e:
            logger.error(f"Error in registration: {e}")
    
    def _chat_interactive(self, scan_results: Dict):
        """Interactive chat with AI about face analysis"""
        if not self.llm_available:
            print("\n✗ LLM not available. Configure API key to enable chat.")
            return
        
        print("\nEnter your question (or 'exit'): ", end='', flush=True)
        try:
            question = input().strip()
            
            if question.lower() in ['exit', 'cancel']:
                print("Chat cancelled")
                return
            
            response = self.llm.chat(question, face_context=scan_results)
            print(f"\n🤖 AI: {response}\n")
        except Exception as e:
            logger.error(f"Error in chat: {e}")
    
    def batch_analyze(self, image_dir: str, output_dir: str = "analysis_results"):
        """
        Batch analyze multiple images
        
        Args:
            image_dir: Directory containing images
            output_dir: Output directory for results
        """
        image_dir = Path(image_dir)
        output_dir = Path(output_dir)
        output_dir.mkdir(exist_ok=True)
        
        image_extensions = ['.jpg', '.jpeg', '.png', '.bmp']
        image_files = []
        for ext in image_extensions:
            image_files.extend(image_dir.glob(f"*{ext}"))
        
        print(f"\nBatch analyzing {len(image_files)} images...")
        
        results = []
        for i, image_path in enumerate(image_files, 1):
            print(f"\nProcessing {i}/{len(image_files)}: {image_path.name}")
            
            result = self.analyze_image(str(image_path))
            if result:
                results.append({
                    'filename': image_path.name,
                    'analysis': result
                })
                
                # Save individual results
                result_file = output_dir / f"{image_path.stem}_analysis.json"
                with open(result_file, 'w') as f:
                    json.dump(result, f, indent=2)
        
        # Save batch summary
        summary_file = output_dir / "batch_summary.json"
        with open(summary_file, 'w') as f:
            json.dump(results, f, indent=2)
        
        print(f"\n✓ Batch analysis complete. Results saved to: {output_dir}")


def main():
    """Main entry point"""
    parser = argparse.ArgumentParser(
        description="Advanced Face Scanning Tool with LLM Integration"
    )
    parser.add_argument(
        '--mode', 
        choices=['camera', 'image', 'batch'],
        default='camera',
        help='Operation mode'
    )
    parser.add_argument(
        '--image', 
        type=str,
        help='Image file path (for image mode)'
    )
    parser.add_argument(
        '--input-dir',
        type=str,
        help='Input directory (for batch mode)'
    )
    parser.add_argument(
        '--output-dir',
        type=str,
        default='analysis_results',
        help='Output directory (for batch mode)'
    )
    parser.add_argument(
        '--llm-provider',
        choices=['openai', 'anthropic'],
        default='openai',
        help='LLM provider'
    )
    parser.add_argument(
        '--llm-model',
        type=str,
        help='LLM model name'
    )
    parser.add_argument(
        '--detection-model',
        choices=['hog', 'cnn'],
        default='hog',
        help='Face detection model (hog=CPU, cnn=GPU)'
    )
    parser.add_argument(
        '--camera-id',
        type=int,
        default=0,
        help='Camera device ID'
    )
    parser.add_argument(
        '--no-video',
        action='store_true',
        help='Disable video display'
    )
    
    args = parser.parse_args()
    
    # Initialize app
    app = FaceScanApp(
        llm_provider=args.llm_provider,
        llm_model=args.llm_model,
        detection_model=args.detection_model
    )
    
    # Run appropriate mode
    if args.mode == 'camera':
        app.run_camera_mode(show_video=not args.no_video)
    
    elif args.mode == 'image':
        if not args.image:
            print("Error: --image required for image mode")
            return
        results = app.analyze_image(args.image)
        print("\n" + "=" * 60)
        print("ANALYSIS RESULTS:")
        print("=" * 60)
        print(json.dumps(results, indent=2))
    
    elif args.mode == 'batch':
        if not args.input_dir:
            print("Error: --input-dir required for batch mode")
            return
        app.batch_analyze(args.input_dir, args.output_dir)


if __name__ == "__main__":
    main()
