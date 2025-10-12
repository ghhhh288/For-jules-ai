"""
Web-based UI for Face Scanning with LLM
Beautiful, modern interface using Gradio
"""

import gradio as gr
import cv2
import numpy as np
from PIL import Image
import json
from datetime import datetime
from typing import Optional
import logging

from face_scanner import FaceScanner
from llm_engine import LLMEngine

logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)


class FaceScanWebUI:
    """Web UI for Face Scanning Application"""
    
    def __init__(self, llm_provider: str = "openai"):
        """Initialize Web UI"""
        self.scanner = FaceScanner(model="hog")
        
        try:
            self.llm = LLMEngine(provider=llm_provider)
            self.llm_available = True
        except Exception as e:
            logger.warning(f"LLM not available: {e}")
            self.llm = None
            self.llm_available = False
        
        self.current_scan = None
        logger.info("Web UI initialized")
    
    def analyze_image(self, image: np.ndarray) -> tuple:
        """
        Analyze uploaded image
        
        Args:
            image: Input image from Gradio
            
        Returns:
            Tuple of (annotated_image, results_text, json_data)
        """
        if image is None:
            return None, "No image provided", "{}"
        
        try:
            # Convert RGB to BGR for OpenCV
            if len(image.shape) == 2:
                image_bgr = cv2.cvtColor(image, cv2.COLOR_GRAY2BGR)
            else:
                image_bgr = cv2.cvtColor(image, cv2.COLOR_RGB2BGR)
            
            # Perform comprehensive scan
            scan_results = self.scanner.comprehensive_scan(image_bgr)
            self.current_scan = scan_results
            
            # Create annotated image
            annotated = self.scanner.draw_annotations(image_bgr, scan_results)
            annotated_rgb = cv2.cvtColor(annotated, cv2.COLOR_BGR2RGB)
            
            # Format results text
            results_text = self._format_results(scan_results)
            
            # JSON output
            json_output = json.dumps(scan_results, indent=2)
            
            return annotated_rgb, results_text, json_output
            
        except Exception as e:
            logger.error(f"Error analyzing image: {e}")
            return None, f"Error: {str(e)}", "{}"
    
    def get_ai_analysis(self) -> str:
        """Get AI analysis of current scan"""
        if not self.llm_available:
            return "❌ LLM not available. Please configure your API key in .env file:\n\n" \
                   "OPENAI_API_KEY=your_key_here\nor\nANTHROPIC_API_KEY=your_key_here"
        
        if not self.current_scan:
            return "⚠️ No scan data available. Please analyze an image first."
        
        try:
            analysis = self.llm.analyze_face_data(self.current_scan)
            return f"🤖 **AI Analysis**\n\n{analysis}"
        except Exception as e:
            logger.error(f"Error in AI analysis: {e}")
            return f"❌ Error generating analysis: {str(e)}"
    
    def chat_with_ai(self, message: str, history: list) -> tuple:
        """
        Chat with AI about face analysis
        
        Args:
            message: User message
            history: Chat history
            
        Returns:
            Tuple of (empty_string, updated_history)
        """
        if not self.llm_available:
            response = "❌ LLM not available. Please configure your API key."
            history.append((message, response))
            return "", history
        
        try:
            response = self.llm.chat(message, face_context=self.current_scan)
            history.append((message, response))
            return "", history
        except Exception as e:
            error_msg = f"❌ Error: {str(e)}"
            history.append((message, error_msg))
            return "", history
    
    def register_face(self, image: np.ndarray, name: str) -> str:
        """
        Register a face with a name
        
        Args:
            image: Input image
            name: Name to register
            
        Returns:
            Status message
        """
        if image is None:
            return "❌ No image provided"
        
        if not name or name.strip() == "":
            return "❌ Please provide a name"
        
        try:
            # Convert RGB to BGR
            if len(image.shape) == 2:
                image_bgr = cv2.cvtColor(image, cv2.COLOR_GRAY2BGR)
            else:
                image_bgr = cv2.cvtColor(image, cv2.COLOR_RGB2BGR)
            
            success = self.scanner.register_face(image_bgr, name.strip())
            
            if success:
                return f"✅ Successfully registered face for: {name}"
            else:
                return "❌ Failed to register face. Make sure a clear face is visible."
                
        except Exception as e:
            logger.error(f"Error registering face: {e}")
            return f"❌ Error: {str(e)}"
    
    def _format_results(self, scan_results: dict) -> str:
        """Format scan results as readable text"""
        total = scan_results.get('total_faces', 0)
        timestamp = scan_results.get('timestamp', '')
        
        if total == 0:
            return "**No faces detected** 😔\n\nTry uploading a clearer image with visible faces."
        
        output = f"# 🎯 Face Scan Results\n\n"
        output += f"**Timestamp:** {timestamp}\n"
        output += f"**Total Faces Detected:** {total}\n\n"
        output += "---\n\n"
        
        for idx, face in enumerate(scan_results.get('faces', []), 1):
            output += f"## 👤 Face {idx}\n\n"
            
            # Identity
            name = face.get('name', 'Unknown')
            output += f"**Identity:** {name}\n\n"
            
            # Features
            features = face.get('features', {})
            output += f"**Demographics:**\n"
            output += f"- Age: {features.get('age', 'N/A')}\n"
            output += f"- Gender: {features.get('gender', 'N/A')}\n"
            output += f"- Ethnicity: {features.get('race', 'N/A')}\n\n"
            
            # Emotion
            emotion = features.get('emotion', 'Unknown')
            output += f"**Primary Emotion:** {emotion} "
            
            # Add emoji based on emotion
            emotion_emojis = {
                'happy': '😊',
                'sad': '😢',
                'angry': '😠',
                'surprise': '😲',
                'fear': '😨',
                'disgust': '🤢',
                'neutral': '😐'
            }
            output += emotion_emojis.get(emotion.lower(), '🙂')
            output += "\n\n"
            
            # Emotion breakdown
            emotion_scores = features.get('emotion_scores', {})
            if emotion_scores:
                output += "**Emotion Analysis:**\n"
                sorted_emotions = sorted(emotion_scores.items(), 
                                       key=lambda x: x[1], reverse=True)
                for emotion_name, score in sorted_emotions[:5]:
                    bar_length = int(score / 5)
                    bar = "█" * bar_length
                    output += f"- {emotion_name}: {bar} {score:.1f}%\n"
                output += "\n"
            
            output += "---\n\n"
        
        return output
    
    def create_interface(self) -> gr.Blocks:
        """Create Gradio interface"""
        
        with gr.Blocks(
            title="Face Scan LLM - Advanced Face Analysis",
            theme=gr.themes.Soft()
        ) as interface:
            
            gr.Markdown("""
            # 🔍 Face Scan LLM - Advanced Face Analysis System
            
            Upload an image or use your webcam to detect and analyze faces with AI-powered insights.
            
            **Features:**
            - 👁️ Real-time face detection
            - 🧠 Age, gender, and emotion analysis
            - 🤖 AI-powered interpretations
            - 💬 Interactive chat about face analysis
            - 📝 Face registration and recognition
            """)
            
            with gr.Tab("📸 Face Analysis"):
                with gr.Row():
                    with gr.Column(scale=1):
                        input_image = gr.Image(
                            label="Upload Image or Use Webcam",
                            sources=["upload", "webcam"],
                            type="numpy"
                        )
                        analyze_btn = gr.Button("🔍 Analyze Face", variant="primary", size="lg")
                        
                        gr.Markdown("### Quick Actions")
                        ai_analysis_btn = gr.Button("🤖 Get AI Analysis", size="sm")
                        
                    with gr.Column(scale=1):
                        output_image = gr.Image(label="Annotated Result")
                        
                with gr.Row():
                    with gr.Column(scale=1):
                        results_text = gr.Markdown(label="Analysis Results")
                    with gr.Column(scale=1):
                        json_output = gr.Code(
                            label="Raw JSON Data",
                            language="json"
                        )
                
                with gr.Row():
                    ai_analysis_output = gr.Markdown(label="AI Analysis")
                
                # Wire up analysis
                analyze_btn.click(
                    fn=self.analyze_image,
                    inputs=[input_image],
                    outputs=[output_image, results_text, json_output]
                )
                
                ai_analysis_btn.click(
                    fn=self.get_ai_analysis,
                    inputs=[],
                    outputs=[ai_analysis_output]
                )
            
            with gr.Tab("💬 AI Chat"):
                gr.Markdown("""
                ### Chat with AI about Face Analysis
                Ask questions about the detected faces, request insights, or discuss the results.
                """)
                
                chatbot = gr.Chatbot(
                    label="AI Assistant",
                    height=400,
                    bubble_full_width=False
                )
                
                with gr.Row():
                    chat_input = gr.Textbox(
                        label="Your Message",
                        placeholder="Ask me anything about the face analysis...",
                        scale=4
                    )
                    chat_btn = gr.Button("Send", scale=1, variant="primary")
                
                chat_examples = gr.Examples(
                    examples=[
                        "What can you tell me about the emotions detected?",
                        "Analyze the demographics of the faces",
                        "What does the facial expression suggest?",
                        "Are there any notable patterns in the data?",
                    ],
                    inputs=chat_input
                )
                
                # Wire up chat
                chat_btn.click(
                    fn=self.chat_with_ai,
                    inputs=[chat_input, chatbot],
                    outputs=[chat_input, chatbot]
                )
                
                chat_input.submit(
                    fn=self.chat_with_ai,
                    inputs=[chat_input, chatbot],
                    outputs=[chat_input, chatbot]
                )
            
            with gr.Tab("📝 Face Registration"):
                gr.Markdown("""
                ### Register Known Faces
                Upload an image and provide a name to register a face for recognition.
                """)
                
                with gr.Row():
                    with gr.Column():
                        register_image = gr.Image(
                            label="Upload Face Image",
                            sources=["upload", "webcam"],
                            type="numpy"
                        )
                        register_name = gr.Textbox(
                            label="Name",
                            placeholder="Enter person's name"
                        )
                        register_btn = gr.Button(
                            "Register Face",
                            variant="primary",
                            size="lg"
                        )
                    
                    with gr.Column():
                        register_output = gr.Markdown(
                            label="Registration Status"
                        )
                        
                        gr.Markdown("""
                        **Registered Faces:**
                        
                        Once you register faces, they will be recognized in future analyses.
                        The system will identify these people by name in subsequent scans.
                        """)
                
                register_btn.click(
                    fn=self.register_face,
                    inputs=[register_image, register_name],
                    outputs=[register_output]
                )
            
            with gr.Tab("ℹ️ About"):
                gr.Markdown("""
                ## About Face Scan LLM
                
                This is an advanced face scanning and analysis system that combines:
                
                ### 🎯 Core Technologies
                - **OpenCV & face_recognition**: Face detection and feature extraction
                - **DeepFace**: Advanced demographic and emotion analysis
                - **LLM Integration**: OpenAI GPT-4 or Anthropic Claude for intelligent insights
                
                ### 🚀 Features
                1. **Face Detection**: Accurate detection of multiple faces
                2. **Feature Analysis**: Age, gender, emotion, and ethnicity estimation
                3. **Facial Landmarks**: Detection of eyes, nose, mouth, etc.
                4. **Face Recognition**: Register and identify known faces
                5. **AI Analysis**: Natural language insights about face data
                6. **Interactive Chat**: Ask questions about analysis results
                
                ### 🔧 Setup
                To enable AI features, create a `.env` file with:
                ```
                OPENAI_API_KEY=your_key_here
                # OR
                ANTHROPIC_API_KEY=your_key_here
                ```
                
                ### 📝 Usage Tips
                - Use clear, well-lit images for best results
                - Face should be clearly visible and not too small
                - Multiple faces can be analyzed simultaneously
                - Register faces for recognition in future scans
                
                ### ⚠️ Privacy & Ethics
                - This tool is for educational and research purposes
                - Always obtain consent before analyzing someone's face
                - Be mindful of privacy and data protection laws
                - Demographic predictions are estimates and may not be accurate
                
                ### 📚 Built With
                - Python 3.8+
                - OpenCV
                - face_recognition / dlib
                - DeepFace
                - TensorFlow
                - OpenAI / Anthropic APIs
                - Gradio
                
                ---
                
                **Version:** 1.0.0 | **License:** MIT
                """)
        
        return interface
    
    def launch(self, share: bool = False, server_port: int = 7860):
        """
        Launch the web interface
        
        Args:
            share: Create public link
            server_port: Port to run on
        """
        interface = self.create_interface()
        interface.launch(
            share=share,
            server_port=server_port,
            server_name="0.0.0.0"
        )


def main():
    """Main entry point for web UI"""
    import argparse
    
    parser = argparse.ArgumentParser(description="Face Scan LLM Web Interface")
    parser.add_argument(
        '--llm-provider',
        choices=['openai', 'anthropic'],
        default='openai',
        help='LLM provider'
    )
    parser.add_argument(
        '--share',
        action='store_true',
        help='Create public Gradio link'
    )
    parser.add_argument(
        '--port',
        type=int,
        default=7860,
        help='Server port'
    )
    
    args = parser.parse_args()
    
    print("\n" + "="*60)
    print("🚀 Starting Face Scan LLM Web Interface")
    print("="*60 + "\n")
    
    ui = FaceScanWebUI(llm_provider=args.llm_provider)
    ui.launch(share=args.share, server_port=args.port)


if __name__ == "__main__":
    main()
