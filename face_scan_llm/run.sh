#!/bin/bash
# Face Scan LLM - Quick Launch Script

echo "=================================="
echo "  Face Scan LLM - Quick Launcher  "
echo "=================================="
echo ""

# Function to show menu
show_menu() {
    echo "Select launch mode:"
    echo ""
    echo "  1) Web Interface (Recommended)"
    echo "  2) Camera Mode"
    echo "  3) Setup/Install"
    echo "  4) Help"
    echo "  5) Exit"
    echo ""
    read -p "Enter choice [1-5]: " choice
}

# Main loop
while true; do
    show_menu
    
    case $choice in
        1)
            echo ""
            echo "Launching Web Interface..."
            echo "Open browser to: http://localhost:7860"
            echo "Press Ctrl+C to stop"
            echo ""
            python web_ui.py
            ;;
        2)
            echo ""
            echo "Launching Camera Mode..."
            echo "Press 'q' to quit"
            echo ""
            python main_app.py --mode camera
            ;;
        3)
            echo ""
            echo "Running setup..."
            python setup.py
            ;;
        4)
            echo ""
            echo "=== Quick Help ==="
            echo ""
            echo "Web Interface: python web_ui.py"
            echo "Camera Mode:   python main_app.py --mode camera"
            echo "Analyze Image: python main_app.py --mode image --image photo.jpg"
            echo "Batch Process: python main_app.py --mode batch --input-dir ./images"
            echo ""
            echo "See README.md for full documentation"
            echo ""
            read -p "Press Enter to continue..."
            ;;
        5)
            echo ""
            echo "Goodbye!"
            exit 0
            ;;
        *)
            echo ""
            echo "Invalid choice. Please select 1-5."
            echo ""
            sleep 2
            ;;
    esac
done
