"""
Setup script for Face Scan LLM
Automated installation and configuration
"""

import subprocess
import sys
import os
from pathlib import Path


def print_header(text):
    """Print formatted header"""
    print("\n" + "=" * 60)
    print(f"  {text}")
    print("=" * 60 + "\n")


def check_python_version():
    """Check Python version"""
    print_header("Checking Python Version")
    version = sys.version_info
    print(f"Python version: {version.major}.{version.minor}.{version.micro}")
    
    if version.major < 3 or (version.major == 3 and version.minor < 8):
        print("❌ Error: Python 3.8 or higher is required")
        return False
    
    print("✅ Python version OK")
    return True


def install_dependencies():
    """Install required packages"""
    print_header("Installing Dependencies")
    
    try:
        print("Installing packages from requirements.txt...")
        subprocess.check_call([
            sys.executable, 
            "-m", 
            "pip", 
            "install", 
            "-r", 
            "requirements.txt"
        ])
        print("\n✅ Dependencies installed successfully")
        return True
    except subprocess.CalledProcessError as e:
        print(f"\n❌ Error installing dependencies: {e}")
        print("\nTry installing manually:")
        print("  pip install -r requirements.txt")
        return False


def create_env_file():
    """Create .env file from template"""
    print_header("Setting Up Configuration")
    
    env_file = Path(".env")
    env_example = Path(".env.example")
    
    if env_file.exists():
        print("⚠️  .env file already exists")
        response = input("Overwrite? (y/N): ").strip().lower()
        if response != 'y':
            print("Keeping existing .env file")
            return True
    
    if not env_example.exists():
        print("⚠️  .env.example not found")
        return False
    
    # Copy template
    with open(env_example, 'r') as f:
        content = f.read()
    
    with open(env_file, 'w') as f:
        f.write(content)
    
    print("✅ Created .env file")
    print("\n⚠️  IMPORTANT: Edit .env and add your API keys:")
    print("     - OPENAI_API_KEY (for GPT-4)")
    print("     - ANTHROPIC_API_KEY (for Claude)")
    return True


def create_directories():
    """Create necessary directories"""
    print_header("Creating Directories")
    
    directories = ['data', 'output', 'models']
    
    for dir_name in directories:
        dir_path = Path(dir_name)
        dir_path.mkdir(exist_ok=True)
        print(f"✅ Created {dir_name}/ directory")
    
    return True


def test_imports():
    """Test if key packages can be imported"""
    print_header("Testing Imports")
    
    test_packages = [
        ('cv2', 'OpenCV'),
        ('face_recognition', 'face_recognition'),
        ('deepface', 'DeepFace'),
        ('gradio', 'Gradio'),
        ('numpy', 'NumPy'),
    ]
    
    all_ok = True
    
    for package, name in test_packages:
        try:
            __import__(package)
            print(f"✅ {name} imported successfully")
        except ImportError as e:
            print(f"❌ Failed to import {name}: {e}")
            all_ok = False
    
    return all_ok


def print_next_steps():
    """Print next steps for user"""
    print_header("Setup Complete! 🎉")
    
    print("""
Next Steps:

1. Configure API Keys (Required for AI features):
   Edit .env file and add your API key:
   
   OPENAI_API_KEY=sk-your-key-here
   # OR
   ANTHROPIC_API_KEY=sk-ant-your-key-here

2. Run the Application:

   Web Interface (Recommended):
     python web_ui.py
   
   Camera Mode:
     python main_app.py --mode camera
   
   Analyze Image:
     python main_app.py --mode image --image your_photo.jpg

3. Read the Documentation:
   See README.md for detailed usage instructions

4. Test the System:
   Try uploading a test image through the web interface

Troubleshooting:
- If you encounter issues with dlib, see README.md
- For GPU acceleration, install tensorflow-gpu
- Check logs for detailed error messages

Enjoy using Face Scan LLM! 🔍
""")


def main():
    """Main setup routine"""
    print("""
╔════════════════════════════════════════════════════════════╗
║                                                            ║
║         Face Scan LLM - Setup & Installation               ║
║                                                            ║
║    Advanced Face Analysis with AI-Powered Insights        ║
║                                                            ║
╚════════════════════════════════════════════════════════════╝
""")
    
    steps = [
        ("Checking Python version", check_python_version),
        ("Installing dependencies", install_dependencies),
        ("Creating configuration", create_env_file),
        ("Creating directories", create_directories),
        ("Testing imports", test_imports),
    ]
    
    for step_name, step_func in steps:
        if not step_func():
            print(f"\n❌ Setup failed at: {step_name}")
            print("Please resolve the issues and run setup again.")
            sys.exit(1)
    
    print_next_steps()


if __name__ == "__main__":
    main()
