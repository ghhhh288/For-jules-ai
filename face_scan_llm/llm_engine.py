"""
LLM Integration Engine for Face Analysis
Provides natural language interface and intelligent analysis
"""

import os
from typing import Dict, List, Optional
import json
import logging
from datetime import datetime

# Support multiple LLM providers
try:
    import openai
    OPENAI_AVAILABLE = True
except ImportError:
    OPENAI_AVAILABLE = False

try:
    import anthropic
    ANTHROPIC_AVAILABLE = True
except ImportError:
    ANTHROPIC_AVAILABLE = False

from dotenv import load_dotenv

load_dotenv()
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)


class LLMEngine:
    """LLM Engine for intelligent face analysis and conversation"""
    
    def __init__(self, provider: str = "openai", model: str = None):
        """
        Initialize LLM Engine
        
        Args:
            provider: LLM provider ('openai', 'anthropic', 'local')
            model: Model name (auto-selects if None)
        """
        self.provider = provider.lower()
        self.conversation_history = []
        
        if self.provider == "openai":
            if not OPENAI_AVAILABLE:
                raise ImportError("OpenAI package not installed. Run: pip install openai")
            self.api_key = os.getenv("OPENAI_API_KEY")
            if not self.api_key:
                logger.warning("OPENAI_API_KEY not found in environment")
            openai.api_key = self.api_key
            self.model = model or "gpt-4-turbo-preview"
            self.client = openai.OpenAI(api_key=self.api_key)
            
        elif self.provider == "anthropic":
            if not ANTHROPIC_AVAILABLE:
                raise ImportError("Anthropic package not installed. Run: pip install anthropic")
            self.api_key = os.getenv("ANTHROPIC_API_KEY")
            if not self.api_key:
                logger.warning("ANTHROPIC_API_KEY not found in environment")
            self.model = model or "claude-3-opus-20240229"
            self.client = anthropic.Anthropic(api_key=self.api_key)
        
        else:
            raise ValueError(f"Unsupported provider: {provider}")
        
        logger.info(f"LLM Engine initialized: {self.provider} - {self.model}")
    
    def analyze_face_data(self, face_data: Dict) -> str:
        """
        Analyze face scan data using LLM
        
        Args:
            face_data: Face scan results from FaceScanner
            
        Returns:
            Natural language analysis
        """
        # Create analysis prompt
        prompt = self._create_analysis_prompt(face_data)
        
        try:
            response = self._call_llm(prompt, system_message=
                "You are an expert face analysis AI assistant. Provide insightful, "
                "respectful, and comprehensive analysis of facial data. Be professional "
                "and empathetic in your responses."
            )
            return response
        except Exception as e:
            logger.error(f"Error in LLM analysis: {e}")
            return f"Analysis error: {str(e)}"
    
    def chat(self, user_message: str, face_context: Optional[Dict] = None) -> str:
        """
        Chat with LLM about face analysis
        
        Args:
            user_message: User's question or comment
            face_context: Optional face scan context
            
        Returns:
            LLM response
        """
        # Build context-aware prompt
        if face_context:
            context_str = self._format_face_context(face_context)
            full_message = f"Context:\n{context_str}\n\nUser: {user_message}"
        else:
            full_message = user_message
        
        # Add to conversation history
        self.conversation_history.append({
            "role": "user",
            "content": full_message,
            "timestamp": datetime.now().isoformat()
        })
        
        try:
            response = self._call_llm_chat(self.conversation_history)
            
            # Add response to history
            self.conversation_history.append({
                "role": "assistant",
                "content": response,
                "timestamp": datetime.now().isoformat()
            })
            
            return response
        except Exception as e:
            logger.error(f"Error in chat: {e}")
            return f"Chat error: {str(e)}"
    
    def _create_analysis_prompt(self, face_data: Dict) -> str:
        """Create detailed analysis prompt from face data"""
        total_faces = face_data.get('total_faces', 0)
        faces = face_data.get('faces', [])
        
        prompt = f"Analyze the following face scan results:\n\n"
        prompt += f"Total faces detected: {total_faces}\n\n"
        
        for idx, face in enumerate(faces, 1):
            prompt += f"Face {idx}:\n"
            prompt += f"- Identity: {face.get('name', 'Unknown')}\n"
            
            features = face.get('features', {})
            prompt += f"- Estimated Age: {features.get('age', 'N/A')}\n"
            prompt += f"- Gender: {features.get('gender', 'N/A')}\n"
            prompt += f"- Primary Emotion: {features.get('emotion', 'N/A')}\n"
            prompt += f"- Ethnicity: {features.get('race', 'N/A')}\n"
            
            emotion_scores = features.get('emotion_scores', {})
            if emotion_scores:
                prompt += f"- Emotion Analysis:\n"
                for emotion, score in sorted(emotion_scores.items(), 
                                            key=lambda x: x[1], reverse=True)[:3]:
                    prompt += f"  * {emotion}: {score:.2f}%\n"
            
            prompt += "\n"
        
        prompt += "\nProvide a comprehensive, insightful analysis of these results. "
        prompt += "Include observations about emotions, demographics, and any notable patterns. "
        prompt += "Be respectful and professional."
        
        return prompt
    
    def _format_face_context(self, face_context: Dict) -> str:
        """Format face context for chat"""
        context = "Current Face Scan Data:\n"
        context += json.dumps(face_context, indent=2)
        return context
    
    def _call_llm(self, prompt: str, system_message: str = None) -> str:
        """Call LLM API"""
        if self.provider == "openai":
            return self._call_openai(prompt, system_message)
        elif self.provider == "anthropic":
            return self._call_anthropic(prompt, system_message)
        else:
            return "Unsupported provider"
    
    def _call_llm_chat(self, messages: List[Dict]) -> str:
        """Call LLM with conversation history"""
        if self.provider == "openai":
            return self._call_openai_chat(messages)
        elif self.provider == "anthropic":
            return self._call_anthropic_chat(messages)
        else:
            return "Unsupported provider"
    
    def _call_openai(self, prompt: str, system_message: str = None) -> str:
        """Call OpenAI API"""
        messages = []
        if system_message:
            messages.append({"role": "system", "content": system_message})
        messages.append({"role": "user", "content": prompt})
        
        try:
            response = self.client.chat.completions.create(
                model=self.model,
                messages=messages,
                temperature=0.7,
                max_tokens=1000
            )
            return response.choices[0].message.content
        except Exception as e:
            logger.error(f"OpenAI API error: {e}")
            # Return mock response if API fails
            return self._generate_mock_analysis(prompt)
    
    def _call_openai_chat(self, messages: List[Dict]) -> str:
        """Call OpenAI chat API"""
        formatted_messages = [
            {"role": "system", "content": "You are an expert face analysis AI assistant."}
        ]
        for msg in messages:
            formatted_messages.append({
                "role": msg["role"],
                "content": msg["content"]
            })
        
        try:
            response = self.client.chat.completions.create(
                model=self.model,
                messages=formatted_messages,
                temperature=0.7,
                max_tokens=800
            )
            return response.choices[0].message.content
        except Exception as e:
            logger.error(f"OpenAI chat error: {e}")
            return self._generate_mock_response(messages[-1]["content"])
    
    def _call_anthropic(self, prompt: str, system_message: str = None) -> str:
        """Call Anthropic Claude API"""
        try:
            response = self.client.messages.create(
                model=self.model,
                max_tokens=1000,
                system=system_message or "You are an expert face analysis AI assistant.",
                messages=[
                    {"role": "user", "content": prompt}
                ]
            )
            return response.content[0].text
        except Exception as e:
            logger.error(f"Anthropic API error: {e}")
            return self._generate_mock_analysis(prompt)
    
    def _call_anthropic_chat(self, messages: List[Dict]) -> str:
        """Call Anthropic chat API"""
        formatted_messages = []
        for msg in messages:
            if msg["role"] in ["user", "assistant"]:
                formatted_messages.append({
                    "role": msg["role"],
                    "content": msg["content"]
                })
        
        try:
            response = self.client.messages.create(
                model=self.model,
                max_tokens=800,
                system="You are an expert face analysis AI assistant.",
                messages=formatted_messages
            )
            return response.content[0].text
        except Exception as e:
            logger.error(f"Anthropic chat error: {e}")
            return self._generate_mock_response(messages[-1]["content"])
    
    def _generate_mock_analysis(self, prompt: str) -> str:
        """Generate mock analysis when API is unavailable"""
        return """
**Face Analysis Summary** (Mock Mode - API Unavailable)

Based on the detected facial features, here's a professional analysis:

**Overall Assessment:**
The facial analysis system has successfully processed the image and extracted 
key demographic and emotional indicators. The data suggests active engagement 
and natural emotional expression.

**Key Observations:**
- Emotional State: The primary emotion detected indicates the subject's current mood
- Demographics: Age and gender estimations provide context for the analysis
- Facial Features: Landmark detection confirms proper face positioning and clarity

**Recommendations:**
For production use, please configure your OpenAI or Anthropic API key in the .env file.
This will enable full AI-powered analysis with detailed insights.

Note: This is a fallback response. Real analysis would provide deeper insights
based on the specific facial data detected.
"""
    
    def _generate_mock_response(self, user_message: str) -> str:
        """Generate mock chat response"""
        return f"""Thank you for your question. I'm currently in mock mode as the LLM API 
is unavailable. To enable full conversational AI capabilities, please:

1. Set your OPENAI_API_KEY or ANTHROPIC_API_KEY in a .env file
2. Ensure you have a valid API subscription
3. Restart the application

Your message: "{user_message[:100]}..."

In full mode, I would provide detailed insights about the face analysis data,
answer questions about the detected features, and engage in meaningful conversation
about the results.
"""
    
    def generate_report(self, face_data: Dict) -> Dict:
        """
        Generate comprehensive report with LLM insights
        
        Args:
            face_data: Face scan results
            
        Returns:
            Structured report dictionary
        """
        analysis = self.analyze_face_data(face_data)
        
        report = {
            'timestamp': datetime.now().isoformat(),
            'raw_data': face_data,
            'llm_analysis': analysis,
            'summary': {
                'total_faces': face_data.get('total_faces', 0),
                'identities': [f.get('name', 'Unknown') for f in face_data.get('faces', [])],
                'emotions': [f.get('features', {}).get('emotion', 'Unknown') 
                           for f in face_data.get('faces', [])],
                'avg_age': self._calculate_avg_age(face_data)
            }
        }
        
        return report
    
    def _calculate_avg_age(self, face_data: Dict) -> float:
        """Calculate average age from face data"""
        ages = []
        for face in face_data.get('faces', []):
            age = face.get('features', {}).get('age', None)
            if age and isinstance(age, (int, float)):
                ages.append(age)
        
        return sum(ages) / len(ages) if ages else 0
    
    def clear_history(self):
        """Clear conversation history"""
        self.conversation_history = []
        logger.info("Conversation history cleared")


if __name__ == "__main__":
    # Test the LLM engine
    print("Testing LLM Engine...")
    try:
        engine = LLMEngine(provider="openai")
        print(f"LLM Engine initialized: {engine.model}")
    except Exception as e:
        print(f"Error: {e}")
        print("To use this module, set OPENAI_API_KEY or ANTHROPIC_API_KEY in .env file")
