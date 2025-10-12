# 🚀 Quick Start Guide

Get up and running with Face Scan LLM in 5 minutes!

## 1️⃣ Install Dependencies

```bash
# Run automated setup
python setup.py

# Or install manually
pip install -r requirements.txt
```

## 2️⃣ Configure API Key (Optional but Recommended)

Create a `.env` file:

```bash
cp .env.example .env
```

Edit `.env` and add your API key:

```env
OPENAI_API_KEY=sk-your-openai-key-here
```

**Get API Keys:**
- OpenAI: https://platform.openai.com/api-keys
- Anthropic: https://console.anthropic.com/

## 3️⃣ Launch the Application

### Option A: Web Interface (Easiest)

```bash
python web_ui.py
```

Open browser to: http://localhost:7860

### Option B: Camera Mode

```bash
python main_app.py --mode camera
```

Press `q` to quit, `s` to save, `a` for AI analysis

### Option C: Analyze Image

```bash
python main_app.py --mode image --image photo.jpg
```

## 4️⃣ Try It Out!

### Web Interface
1. Click "Upload Image or Use Webcam"
2. Select a photo with a face
3. Click "🔍 Analyze Face"
4. View results and click "🤖 Get AI Analysis"
5. Try the chat feature!

### Camera Mode
1. Position your face in view
2. Wait for automatic analysis
3. Press `a` for detailed AI insights
4. Press `r` to register your face
5. Press `c` to chat with AI

## 📝 Common Commands

```bash
# Web UI with public link
python web_ui.py --share

# Use different port
python web_ui.py --port 8080

# Use Claude instead of GPT-4
python web_ui.py --llm-provider anthropic

# Batch process multiple images
python main_app.py --mode batch --input-dir ./photos --output-dir ./results

# Use GPU acceleration (if available)
python main_app.py --detection-model cnn
```

## ⚡ Tips

- **Better Results**: Use clear, well-lit photos
- **Multiple Faces**: System handles multiple people
- **Privacy**: Face data is processed locally
- **No API Key**: System works without API keys (limited AI features)
- **Speed**: Use `--detection-model hog` for faster processing

## 🆘 Quick Troubleshooting

**Can't install dlib?**
```bash
# macOS
brew install cmake
pip install dlib

# Ubuntu
sudo apt-get install cmake
pip install dlib
```

**Camera not working?**
```bash
python main_app.py --mode camera --camera-id 1
```

**Out of memory?**
```bash
python main_app.py --detection-model hog
```

## 📚 Learn More

- Full documentation: `README.md`
- Configuration: `config.py`
- API reference: See docstrings in code

## 🎯 Next Steps

1. ✅ Register your face for recognition
2. ✅ Try different emotions
3. ✅ Test with multiple people
4. ✅ Explore the chat feature
5. ✅ Process a batch of photos

**Happy analyzing! 🔍**
