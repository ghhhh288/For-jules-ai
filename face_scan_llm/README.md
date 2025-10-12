# 🔍 Face Scan LLM - Advanced Face Analysis System

A comprehensive face scanning and analysis tool powered by computer vision and Large Language Models (LLMs). This system combines state-of-the-art face detection, demographic analysis, emotion recognition, and AI-powered insights in a beautiful, easy-to-use interface.

![Python Version](https://img.shields.io/badge/python-3.8%2B-blue)
![License](https://img.shields.io/badge/license-MIT-green)

## ✨ Features

### 🎯 Core Capabilities
- **Multi-Face Detection**: Detect and analyze multiple faces simultaneously
- **Demographic Analysis**: Estimate age, gender, and ethnicity
- **Emotion Recognition**: Detect emotions with confidence scores
- **Facial Landmarks**: Identify key facial features (eyes, nose, mouth, etc.)
- **Face Recognition**: Register and identify known faces
- **Real-time Processing**: Live camera feed analysis

### 🤖 AI Integration
- **LLM-Powered Insights**: Get natural language analysis from GPT-4 or Claude
- **Interactive Chat**: Ask questions about face analysis results
- **Contextual Understanding**: AI considers full facial context
- **Professional Reports**: Generate comprehensive analysis reports

### 🖥️ Multiple Interfaces
- **Web UI**: Beautiful Gradio-based web interface
- **Command Line**: Powerful CLI for batch processing
- **Camera Mode**: Real-time face scanning from webcam
- **Batch Processing**: Analyze multiple images at once

## 📦 Installation

### Prerequisites
- Python 3.8 or higher
- pip package manager
- Webcam (optional, for camera mode)

### Step 1: Clone or Download

```bash
cd face_scan_llm
```

### Step 2: Install Dependencies

```bash
pip install -r requirements.txt
```

**Note**: Installing `dlib` may require compilation. On some systems:

**macOS:**
```bash
brew install cmake
pip install dlib
```

**Ubuntu/Debian:**
```bash
sudo apt-get install build-essential cmake
pip install dlib
```

**Windows:**
```bash
pip install cmake
pip install dlib
```

### Step 3: Configure API Keys

Create a `.env` file in the project directory:

```bash
cp .env.example .env
```

Edit `.env` and add your API key:

```env
# For OpenAI (GPT-4)
OPENAI_API_KEY=sk-your-key-here

# OR for Anthropic (Claude)
ANTHROPIC_API_KEY=sk-ant-your-key-here
```

**Note**: The system works without API keys, but AI features will be limited to mock responses.

## 🚀 Quick Start

### Web Interface (Recommended)

Launch the beautiful web UI:

```bash
python web_ui.py
```

Then open your browser to: `http://localhost:7860`

**Options:**
```bash
# Use different port
python web_ui.py --port 8080

# Create public link (shareable URL)
python web_ui.py --share

# Use Anthropic Claude instead of OpenAI
python web_ui.py --llm-provider anthropic
```

### Camera Mode

Real-time face scanning with your webcam:

```bash
python main_app.py --mode camera
```

**Interactive Commands (during camera mode):**
- `q` - Quit
- `s` - Save current analysis
- `a` - Get AI analysis
- `r` - Register face
- `c` - Chat with AI

### Analyze Single Image

```bash
python main_app.py --mode image --image path/to/your/image.jpg
```

### Batch Processing

Analyze multiple images:

```bash
python main_app.py --mode batch --input-dir ./images --output-dir ./results
```

## 📖 Usage Examples

### Python API

```python
from face_scanner import FaceScanner
from llm_engine import LLMEngine
import cv2

# Initialize
scanner = FaceScanner(model="hog")
llm = LLMEngine(provider="openai")

# Load and analyze image
image = cv2.imread("photo.jpg")
results = scanner.comprehensive_scan(image)

# Get AI insights
analysis = llm.analyze_face_data(results)
print(analysis)

# Draw annotations
annotated = scanner.draw_annotations(image, results)
cv2.imwrite("annotated.jpg", annotated)
```

### Face Registration

```python
# Register a known face
scanner.register_face(image, "John Doe")

# Later, the system will recognize this person
results = scanner.comprehensive_scan(new_image)
print(results['faces'][0]['name'])  # "John Doe"
```

### Interactive Chat

```python
llm = LLMEngine()

# Chat about analysis
response = llm.chat(
    "What emotions are detected?",
    face_context=results
)
print(response)
```

## 🏗️ Architecture

```
face_scan_llm/
├── face_scanner.py      # Core face detection & analysis
├── llm_engine.py        # LLM integration & chat
├── main_app.py          # CLI application
├── web_ui.py            # Gradio web interface
├── config.py            # Configuration management
├── requirements.txt     # Python dependencies
├── .env.example         # Environment template
└── README.md           # This file
```

### Component Overview

#### FaceScanner (`face_scanner.py`)
- Face detection using `face_recognition` and `dlib`
- Feature analysis with `DeepFace`
- Facial landmark detection
- Face encoding and recognition
- Image annotation

#### LLMEngine (`llm_engine.py`)
- OpenAI GPT-4 integration
- Anthropic Claude integration
- Context-aware conversations
- Analysis report generation
- Conversation history management

#### Web UI (`web_ui.py`)
- Modern Gradio interface
- Multiple tabs: Analysis, Chat, Registration
- Real-time image processing
- Interactive visualizations

#### Main App (`main_app.py`)
- CLI interface
- Camera capture
- Batch processing
- File I/O operations

## 🎨 Web UI Features

### Face Analysis Tab
- Upload images or use webcam
- View annotated results
- See detailed demographic and emotion analysis
- Export raw JSON data

### AI Chat Tab
- Ask questions about detected faces
- Get contextual insights
- Natural language interaction
- Conversation history

### Face Registration Tab
- Register known faces
- Name and store face encodings
- Enable future recognition

## 🔧 Configuration

Edit `config.py` or set environment variables:

### LLM Settings
```python
LLM_PROVIDER = "openai"  # or "anthropic"
```

### Detection Settings
```python
DETECTION_MODEL = "hog"  # Fast, CPU-based
# or
DETECTION_MODEL = "cnn"  # More accurate, requires GPU
```

### Analysis Settings
```python
ANALYZE_AGE = True
ANALYZE_GENDER = True
ANALYZE_EMOTION = True
ANALYZE_RACE = True
DETECT_LANDMARKS = True
```

## 📊 Output Formats

### JSON Structure

```json
{
  "timestamp": "2024-01-15T10:30:00",
  "total_faces": 2,
  "faces": [
    {
      "id": 0,
      "name": "John Doe",
      "location": {
        "top": 100,
        "right": 300,
        "bottom": 250,
        "left": 150
      },
      "features": {
        "age": 32,
        "gender": "Man",
        "emotion": "happy",
        "race": "white",
        "emotion_scores": {
          "happy": 85.5,
          "neutral": 10.2,
          "surprise": 4.3
        }
      },
      "landmarks": {
        "left_eye": [[x1, y1], [x2, y2], ...],
        "right_eye": [[x1, y1], [x2, y2], ...],
        "nose_bridge": [...],
        "chin": [...]
      }
    }
  ]
}
```

## 🔒 Privacy & Ethics

### Important Considerations

⚠️ **Always obtain consent** before analyzing someone's face

⚠️ **Respect privacy laws** (GDPR, CCPA, etc.)

⚠️ **Be aware of bias** - demographic predictions are estimates and may not be accurate

⚠️ **Secure data storage** - Handle facial data responsibly

### Best Practices
- Use for educational and research purposes
- Don't store facial data without consent
- Be transparent about how data is used
- Consider the ethical implications
- Implement proper access controls

## 🐛 Troubleshooting

### Common Issues

**Problem**: `dlib` installation fails
```bash
# macOS
brew install cmake
pip install dlib

# Ubuntu
sudo apt-get install build-essential cmake libopenblas-dev liblapack-dev
pip install dlib
```

**Problem**: Camera not detected
```bash
# Try different camera ID
python main_app.py --mode camera --camera-id 1
```

**Problem**: Out of memory
```bash
# Use HOG model instead of CNN
python main_app.py --detection-model hog
```

**Problem**: LLM API errors
- Check your API key in `.env`
- Verify you have API credits
- Check internet connection
- System will fall back to mock responses

### GPU Acceleration

For faster processing with NVIDIA GPUs:

```bash
pip install tensorflow-gpu
# Use CNN model
python main_app.py --detection-model cnn
```

## 📝 API Reference

### FaceScanner Class

```python
scanner = FaceScanner(model="hog")

# Detect faces
locations = scanner.detect_faces(image)

# Comprehensive analysis
results = scanner.comprehensive_scan(image)

# Register face
scanner.register_face(image, "Name")

# Recognize faces
names = scanner.recognize_faces(image, locations)

# Draw annotations
annotated = scanner.draw_annotations(image, results)
```

### LLMEngine Class

```python
llm = LLMEngine(provider="openai", model="gpt-4-turbo-preview")

# Analyze face data
analysis = llm.analyze_face_data(scan_results)

# Chat
response = llm.chat("Question", face_context=scan_results)

# Generate report
report = llm.generate_report(scan_results)

# Clear history
llm.clear_history()
```

## 🤝 Contributing

Contributions are welcome! Areas for improvement:

- Additional LLM providers (Cohere, Mistral, etc.)
- More face analysis features
- Performance optimizations
- Better error handling
- Extended documentation
- Test coverage

## 📄 License

This project is licensed under the MIT License - see below:

```
MIT License

Copyright (c) 2024

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

## 🙏 Acknowledgments

This project is built on the shoulders of giants:

- **OpenCV** - Computer vision foundation
- **face_recognition** - Face detection library by Adam Geitgey
- **dlib** - Machine learning toolkit
- **DeepFace** - Facial analysis framework
- **OpenAI** - GPT models
- **Anthropic** - Claude models
- **Gradio** - Web UI framework
- **TensorFlow** - Deep learning backend

## 📞 Support

For issues, questions, or suggestions:
- Check the troubleshooting section
- Review existing GitHub issues
- Create a new issue with detailed information

## 🗺️ Roadmap

Future enhancements:

- [ ] Video file analysis
- [ ] Real-time emotion tracking graphs
- [ ] Multi-language support
- [ ] Face mask detection
- [ ] Age progression prediction
- [ ] Celebrity lookalike matching
- [ ] REST API server
- [ ] Docker containerization
- [ ] Mobile app (React Native)

## 📈 Performance

Typical performance on modern hardware:

| Mode | Model | FPS | Accuracy |
|------|-------|-----|----------|
| HOG (CPU) | Fast | 15-30 | Good |
| CNN (GPU) | Accurate | 30-60 | Excellent |

Analysis time per face:
- Detection: 50-200ms
- Feature analysis: 300-800ms
- LLM analysis: 2-5 seconds

## 🎓 Educational Use

This tool is excellent for:
- Learning computer vision
- Understanding AI/ML pipelines
- Exploring LLM integration
- Teaching responsible AI development
- Research projects

---

**Built with ❤️ using Python, OpenCV, and AI**

*Remember: With great power comes great responsibility. Use facial recognition technology ethically and legally.*
