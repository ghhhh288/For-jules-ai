"""
Configuration Module for Face Scan LLM
Centralized configuration management
"""

import os
from pathlib import Path
from typing import Dict, Any
from dotenv import load_dotenv

# Load environment variables
load_dotenv()


class Config:
    """Application configuration"""
    
    # Project paths
    PROJECT_ROOT = Path(__file__).parent
    DATA_DIR = PROJECT_ROOT / "data"
    OUTPUT_DIR = PROJECT_ROOT / "output"
    MODELS_DIR = PROJECT_ROOT / "models"
    
    # LLM Configuration
    LLM_PROVIDER = os.getenv("DEFAULT_LLM_PROVIDER", "openai")
    OPENAI_API_KEY = os.getenv("OPENAI_API_KEY", "")
    ANTHROPIC_API_KEY = os.getenv("ANTHROPIC_API_KEY", "")
    
    # OpenAI Models
    OPENAI_MODELS = {
        "gpt-4-turbo": "gpt-4-turbo-preview",
        "gpt-4": "gpt-4",
        "gpt-3.5": "gpt-3.5-turbo"
    }
    
    # Anthropic Models
    ANTHROPIC_MODELS = {
        "claude-3-opus": "claude-3-opus-20240229",
        "claude-3-sonnet": "claude-3-sonnet-20240229",
        "claude-3-haiku": "claude-3-haiku-20240307"
    }
    
    # Face Detection Configuration
    DETECTION_MODEL = os.getenv("DEFAULT_DETECTION_MODEL", "hog")  # 'hog' or 'cnn'
    FACE_RECOGNITION_TOLERANCE = 0.6
    
    # Analysis Settings
    ANALYZE_AGE = True
    ANALYZE_GENDER = True
    ANALYZE_EMOTION = True
    ANALYZE_RACE = True
    DETECT_LANDMARKS = True
    
    # Camera Settings
    DEFAULT_CAMERA_ID = 0
    CAMERA_FRAME_WIDTH = 640
    CAMERA_FRAME_HEIGHT = 480
    ANALYSIS_INTERVAL = 30  # Frames between full analysis
    
    # Web UI Settings
    WEB_UI_PORT = int(os.getenv("WEB_UI_PORT", "7860"))
    WEB_UI_SHARE = False
    WEB_UI_THEME = "soft"
    
    # Performance Settings
    MAX_FACES_PER_IMAGE = 10
    IMAGE_MAX_SIZE = (1920, 1080)
    THUMBNAIL_SIZE = (640, 480)
    
    # Logging
    LOG_LEVEL = "INFO"
    LOG_FORMAT = "%(asctime)s - %(name)s - %(levelname)s - %(message)s"
    
    @classmethod
    def ensure_directories(cls):
        """Create necessary directories"""
        cls.DATA_DIR.mkdir(exist_ok=True)
        cls.OUTPUT_DIR.mkdir(exist_ok=True)
        cls.MODELS_DIR.mkdir(exist_ok=True)
    
    @classmethod
    def get_llm_config(cls) -> Dict[str, Any]:
        """Get LLM configuration"""
        return {
            "provider": cls.LLM_PROVIDER,
            "openai_key": cls.OPENAI_API_KEY,
            "anthropic_key": cls.ANTHROPIC_API_KEY,
            "openai_models": cls.OPENAI_MODELS,
            "anthropic_models": cls.ANTHROPIC_MODELS
        }
    
    @classmethod
    def get_scanner_config(cls) -> Dict[str, Any]:
        """Get scanner configuration"""
        return {
            "model": cls.DETECTION_MODEL,
            "tolerance": cls.FACE_RECOGNITION_TOLERANCE,
            "analyze_age": cls.ANALYZE_AGE,
            "analyze_gender": cls.ANALYZE_GENDER,
            "analyze_emotion": cls.ANALYZE_EMOTION,
            "analyze_race": cls.ANALYZE_RACE,
            "detect_landmarks": cls.DETECT_LANDMARKS,
            "max_faces": cls.MAX_FACES_PER_IMAGE
        }
    
    @classmethod
    def validate(cls) -> bool:
        """Validate configuration"""
        issues = []
        
        # Check API keys
        if not cls.OPENAI_API_KEY and not cls.ANTHROPIC_API_KEY:
            issues.append("Warning: No LLM API keys configured. AI features will be limited.")
        
        # Check detection model
        if cls.DETECTION_MODEL not in ["hog", "cnn"]:
            issues.append(f"Invalid DETECTION_MODEL: {cls.DETECTION_MODEL}")
        
        if issues:
            print("\n⚠️  Configuration Issues:")
            for issue in issues:
                print(f"  - {issue}")
            return False
        
        return True


# Initialize directories on import
Config.ensure_directories()


if __name__ == "__main__":
    print("Face Scan LLM Configuration")
    print("=" * 50)
    print(f"Project Root: {Config.PROJECT_ROOT}")
    print(f"LLM Provider: {Config.LLM_PROVIDER}")
    print(f"Detection Model: {Config.DETECTION_MODEL}")
    print(f"Web UI Port: {Config.WEB_UI_PORT}")
    print("=" * 50)
    print("\nValidating configuration...")
    Config.validate()
