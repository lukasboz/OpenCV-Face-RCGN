
 #ifndef PIN_H
 #define PIN_H
 
 // Forward declarations to avoid unnecessary includes
 #include <iostream>
 #include <QWidget>
 #include <QMessageBox>
 #include <QTimer>
 #include <QVBoxLayout>
 #include <QLabel>
 #include <QFrame>
 #include <QLineEdit>
 #include <QPushButton>
 #include <QInputDialog>
 
 #include "EditProfile.h"
 
 
 
 class QFrame;
 class QLineEdit; // Forward declaration to avoid unnecessary includes
 class QPushButton; // Forward declaration for QPushButton
 
 /**
 * @file pin.h
 * @author Kevin Russel
 * @author Woorim Cho 
 * @brief Header file for the pin class, which handles user authentication via PIN input
 * 
 * This Header file defines the pin class which creates a PinWindow when called and handels 
 * the authentication of the user via password.
 */

 class PinWindow : public QWidget {
     Q_OBJECT
 
 public:
     /**
      * @brief Construct a new Pin Window initializing the variables and setting up the UI layout
      * 
      * @param parent Pointer to the parent widget
      */
     explicit PinWindow(QWidget *parent = nullptr);
 
 private:
     /**
      * @brief Construct a new Pin Window which will be the main frame containing all UI elements
      * 
      * @return Pointer to the constructed QFrame
      */
     QFrame* createMainFrame();  // Creates the main frame
 
     /**
      * @brief Constructs the top section of the main frame with a warning message 
      * 
      * @return QFrame* Pointer to the constructed QFrame
      */
     QFrame* createTopSection(); // Creates the warning label section
 
     /**
      * @brief Construct the middle section of the main frame with an input field and a button
      * 
      * @return QFrame* Pointer to the constructed QFrame
      */
     QFrame* createMiddleSection(); // Creates the input field and button section
 
     /**
      * @brief Validates if the inputted password is correct while also handeling incorrect attempts
      * 
      * @param input The user's entered password
      * @param enterButton The button used to send the signal to validate the input
      */
     void validatePassword(const QString &input, QPushButton *enterButton); // Handles password validation
 
     int countdown;          // Current countdown value for timer when locked out
     int counter;            // Login attempt counter
     QMessageBox *popup;     // Pointer to the Message Box that informs user of locked status
     QTimer *timer;          // Pointer to the timer used during the locked status
 
 private slots:
     /**
      * @brief Updates the countdown timer every second and uses it to update the text in the popup. 
      * Upon reaching 0, closes and deletes the timer, deletes the message box, and resets attempt counter
      * 
      */
     void updateCountdown();
 };
 
 #endif // PIN_H
 