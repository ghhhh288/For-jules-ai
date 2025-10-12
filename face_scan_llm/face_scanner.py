"""
Advanced Face Scanning Module
Provides comprehensive face detection, recognition, and feature analysis
"""

import cv2
import face_recognition
import numpy as np
from deepface import DeepFace
from typing import List, Dict, Tuple, Optional
import logging
from datetime import datetime

logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)


class FaceScanner:
    """Advanced face scanning and analysis system"""
    
    def __init__(self, model: str = "hog"):
        """
        Initialize face scanner
        
        Args:
            model: Detection model ('hog' for CPU, 'cnn' for GPU)
        """
        self.model = model
        self.known_faces = []
        self.known_names = []
        self.face_cascade = cv2.CascadeClassifier(
            cv2.data.haarcascades + 'haarcascade_frontalface_default.xml'
        )
        self.eye_cascade = cv2.CascadeClassifier(
            cv2.data.haarcascades + 'haarcascade_eye.xml'
        )
        logger.info(f"FaceScanner initialized with model: {model}")
    
    def detect_faces(self, image: np.ndarray) -> List[Tuple[int, int, int, int]]:
        """
        Detect faces in image
        
        Args:
            image: Input image (BGR format)
            
        Returns:
            List of face locations as (top, right, bottom, left)
        """
        try:
            # Convert BGR to RGB for face_recognition
            rgb_image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
            face_locations = face_recognition.face_locations(rgb_image, model=self.model)
            logger.info(f"Detected {len(face_locations)} faces")
            return face_locations
        except Exception as e:
            logger.error(f"Error detecting faces: {e}")
            return []
    
    def get_face_encodings(self, image: np.ndarray, 
                          face_locations: List[Tuple[int, int, int, int]]) -> List[np.ndarray]:
        """
        Get face encodings for detected faces
        
        Args:
            image: Input image (BGR format)
            face_locations: List of face locations
            
        Returns:
            List of face encodings
        """
        try:
            rgb_image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
            encodings = face_recognition.face_encodings(rgb_image, face_locations)
            return encodings
        except Exception as e:
            logger.error(f"Error getting face encodings: {e}")
            return []
    
    def analyze_face_features(self, image: np.ndarray, 
                            face_location: Tuple[int, int, int, int]) -> Dict:
        """
        Analyze facial features using DeepFace
        
        Args:
            image: Input image (BGR format)
            face_location: Face location (top, right, bottom, left)
            
        Returns:
            Dictionary with age, gender, emotion, race
        """
        try:
            top, right, bottom, left = face_location
            face_image = image[top:bottom, left:right]
            
            # Analyze using DeepFace
            analysis = DeepFace.analyze(
                face_image, 
                actions=['age', 'gender', 'emotion', 'race'],
                enforce_detection=False,
                silent=True
            )
            
            # Extract first result if list
            if isinstance(analysis, list):
                analysis = analysis[0]
            
            return {
                'age': analysis.get('age', 'Unknown'),
                'gender': analysis.get('dominant_gender', 'Unknown'),
                'emotion': analysis.get('dominant_emotion', 'Unknown'),
                'race': analysis.get('dominant_race', 'Unknown'),
                'emotion_scores': analysis.get('emotion', {}),
                'confidence': {
                    'gender': analysis.get('gender', {}).get(analysis.get('dominant_gender', ''), 0),
                }
            }
        except Exception as e:
            logger.error(f"Error analyzing face features: {e}")
            return {
                'age': 'Unknown',
                'gender': 'Unknown',
                'emotion': 'Unknown',
                'race': 'Unknown',
                'emotion_scores': {},
                'confidence': {}
            }
    
    def detect_facial_landmarks(self, image: np.ndarray, 
                               face_locations: List[Tuple[int, int, int, int]]) -> List[Dict]:
        """
        Detect facial landmarks (eyes, nose, mouth, etc.)
        
        Args:
            image: Input image (BGR format)
            face_locations: List of face locations
            
        Returns:
            List of landmark dictionaries
        """
        try:
            rgb_image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
            landmarks = face_recognition.face_landmarks(rgb_image, face_locations)
            return landmarks
        except Exception as e:
            logger.error(f"Error detecting landmarks: {e}")
            return []
    
    def register_face(self, image: np.ndarray, name: str) -> bool:
        """
        Register a face with a name for recognition
        
        Args:
            image: Input image with face
            name: Name to associate with face
            
        Returns:
            True if successful, False otherwise
        """
        try:
            face_locations = self.detect_faces(image)
            if not face_locations:
                logger.warning("No face detected for registration")
                return False
            
            encodings = self.get_face_encodings(image, face_locations)
            if not encodings:
                logger.warning("Could not generate face encoding")
                return False
            
            self.known_faces.append(encodings[0])
            self.known_names.append(name)
            logger.info(f"Registered face for: {name}")
            return True
        except Exception as e:
            logger.error(f"Error registering face: {e}")
            return False
    
    def recognize_faces(self, image: np.ndarray, 
                       face_locations: List[Tuple[int, int, int, int]],
                       tolerance: float = 0.6) -> List[str]:
        """
        Recognize faces by comparing to registered faces
        
        Args:
            image: Input image
            face_locations: List of face locations
            tolerance: Recognition tolerance (lower = stricter)
            
        Returns:
            List of names (or "Unknown")
        """
        if not self.known_faces:
            return ["Unknown"] * len(face_locations)
        
        try:
            encodings = self.get_face_encodings(image, face_locations)
            names = []
            
            for encoding in encodings:
                matches = face_recognition.compare_faces(
                    self.known_faces, encoding, tolerance=tolerance
                )
                name = "Unknown"
                
                # Calculate face distances
                face_distances = face_recognition.face_distance(self.known_faces, encoding)
                if len(face_distances) > 0:
                    best_match_index = np.argmin(face_distances)
                    if matches[best_match_index]:
                        name = self.known_names[best_match_index]
                
                names.append(name)
            
            return names
        except Exception as e:
            logger.error(f"Error recognizing faces: {e}")
            return ["Unknown"] * len(face_locations)
    
    def comprehensive_scan(self, image: np.ndarray) -> Dict:
        """
        Perform comprehensive face scanning and analysis
        
        Args:
            image: Input image (BGR format)
            
        Returns:
            Dictionary with complete analysis results
        """
        results = {
            'timestamp': datetime.now().isoformat(),
            'faces': [],
            'total_faces': 0
        }
        
        try:
            # Detect faces
            face_locations = self.detect_faces(image)
            results['total_faces'] = len(face_locations)
            
            if not face_locations:
                logger.info("No faces detected")
                return results
            
            # Get landmarks
            landmarks = self.detect_facial_landmarks(image, face_locations)
            
            # Recognize faces
            names = self.recognize_faces(image, face_locations)
            
            # Analyze each face
            for idx, (face_location, name) in enumerate(zip(face_locations, names)):
                face_data = {
                    'id': idx,
                    'location': {
                        'top': face_location[0],
                        'right': face_location[1],
                        'bottom': face_location[2],
                        'left': face_location[3]
                    },
                    'name': name,
                    'landmarks': landmarks[idx] if idx < len(landmarks) else {},
                    'features': self.analyze_face_features(image, face_location)
                }
                results['faces'].append(face_data)
            
            logger.info(f"Comprehensive scan complete: {len(face_locations)} faces analyzed")
            return results
            
        except Exception as e:
            logger.error(f"Error in comprehensive scan: {e}")
            return results
    
    def draw_annotations(self, image: np.ndarray, scan_results: Dict) -> np.ndarray:
        """
        Draw face annotations on image
        
        Args:
            image: Input image
            scan_results: Results from comprehensive_scan
            
        Returns:
            Annotated image
        """
        annotated = image.copy()
        
        for face in scan_results.get('faces', []):
            loc = face['location']
            features = face['features']
            name = face['name']
            
            # Draw rectangle
            cv2.rectangle(
                annotated,
                (loc['left'], loc['top']),
                (loc['right'], loc['bottom']),
                (0, 255, 0),
                2
            )
            
            # Prepare text
            labels = [
                f"Name: {name}",
                f"Age: {features.get('age', 'N/A')}",
                f"Gender: {features.get('gender', 'N/A')}",
                f"Emotion: {features.get('emotion', 'N/A')}"
            ]
            
            # Draw labels
            y_offset = loc['top'] - 10
            for label in labels:
                y_offset -= 25
                if y_offset < 0:
                    y_offset = loc['bottom'] + 25
                
                cv2.putText(
                    annotated,
                    label,
                    (loc['left'], y_offset),
                    cv2.FONT_HERSHEY_SIMPLEX,
                    0.6,
                    (0, 255, 0),
                    2
                )
            
            # Draw landmarks
            if face.get('landmarks'):
                for feature, points in face['landmarks'].items():
                    for point in points:
                        cv2.circle(annotated, point, 2, (255, 0, 0), -1)
        
        return annotated


if __name__ == "__main__":
    # Test the scanner
    scanner = FaceScanner()
    print("Face Scanner initialized successfully!")
