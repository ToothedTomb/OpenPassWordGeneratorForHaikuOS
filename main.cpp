#include <Application.h>  // Provides the main application class
#include <Window.h>       // Provides the main window class
#include <View.h>         // Base class for all viewable UI elements
#include <Button.h>       // Button widget for user interaction
#include <TextControl.h>  // Text box widget for input and output
#include <String.h>       // Haiku's string utility class
#include <Clipboard.h>    // Handles clipboard operations
#include <random>         // For generating random passwords
#include <string>         // Standard string library
#include <StringView.h> //This allows us to add the heading. 

// Function to generate a random password of a given length
std::string generate_password(int length) {
    const char charset[] =
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()";
    std::random_device rd; // Seed for randomness
    std::mt19937 generator(rd()); // Random number generator
    std::uniform_int_distribution<size_t> distribution(0, sizeof(charset) - 2);

    std::string password; // The generated password
    for (int i = 0; i < length; i++) {
        password += charset[distribution(generator)];
    }
    return password; // Return the generated password
}

// Custom window class for the password generator
class PasswordGeneratorWindow : public BWindow {
public:
    PasswordGeneratorWindow()
        : BWindow(BRect(100, 100, 510, 300), "Open Password Generator", B_TITLED_WINDOW, B_QUIT_ON_WINDOW_CLOSE) {
        
        // Create the main view (background) for the window
        BView* view = new BView(Bounds(), "view", B_FOLLOW_ALL_SIDES, B_WILL_DRAW);
        AddChild(view); // Add the view to the window

		BStringView* heading = new BStringView(BRect(10,10,400,30),"heading", "Open Password Generator:");
		heading->SetFont(be_bold_font); // Set the heading font to be bold
		view-> AddChild(heading);
        
        // Create entry for password length input
        lengthControl = new BTextControl(BRect(10, 50, 400, 80), "lengthControl", "Enter password length:", "", NULL);
        view->AddChild(lengthControl); // Add to the view

        // Create entry to display the generated password
        passwordControl = new BTextControl(BRect(10, 100, 400, 130), "passwordControl", "Generated Password:", "", NULL);
        view->AddChild(passwordControl); // Add to the view

        // Create a button to generate the password
        BButton* generateButton = new BButton(BRect(10, 150, 200, 180), "generateButton", "Generate Password", new BMessage('GENP'));
        view->AddChild(generateButton); // Add to the view

        // Create a button to copy the password to the clipboard
        BButton* copyButton = new BButton(BRect(210, 150, 400, 180), "copyButton", "Copy to Clipboard", new BMessage('COPY'));
        view->AddChild(copyButton); // Add to the view
    }

    // Handle messages sent by buttons
    void MessageReceived(BMessage* msg) override {
        switch (msg->what) {
            case 'GENP': { // Message from "Generate Password" button
                const char* lenText = lengthControl->Text(); // Get input from the length control
                try {
                    int length = std::stoi(lenText); // Convert input to integer
                    std::string password = generate_password(length); // Generate the password
                    passwordControl->SetText(password.c_str()); // Display the generated password
                } catch (...) {
                    passwordControl->SetText("Error code 1: Invalid length"); // Handle invalid input
                }
                break;
            }
            case 'COPY': { // Message from "Copy to Clipboard" button
                const char* password = passwordControl->Text(); // Get the displayed password
                if (strlen(password) > 0) {
                    // Access clipboard and copy the password
                    if (be_clipboard->Lock()) {
                        be_clipboard->Clear(); // Clear existing clipboard data
                        BMessage* clip = be_clipboard->Data(); // Get clipboard data
                        if (clip) {
                            clip->AddData("text/plain", B_MIME_TYPE, password, strlen(password)); // Add password to clipboard
                            be_clipboard->Commit(); // Commit changes to clipboard
                        }
                        be_clipboard->Unlock(); // Unlock clipboard
                    }
                }
                break;
            }
            default:
                BWindow::MessageReceived(msg); // Pass unhandled messages to the parent class
                break;
        }
    }

private:
    BTextControl* lengthControl;   // Text input for password length
    BTextControl* passwordControl; // Text display for generated password
};

// Main application class
class PasswordGeneratorApp : public BApplication {
public:
    PasswordGeneratorApp()
        : BApplication("application/x-vnd.PasswordGenerator") {}

    void ReadyToRun() override {
        // Create and show the main window
        PasswordGeneratorWindow* window = new PasswordGeneratorWindow();
        window->Show();
    }
};

// Entry point of the application
int main() {
    PasswordGeneratorApp app; // Initialize the application
    app.Run(); // Run the application loop
    return 0;
}

