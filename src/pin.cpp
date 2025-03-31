/**
 * @file pin.cpp
 * @author Kevin Russel
 * @author Woorim Cho
 * @brief This class implements the PinWindow class to authenticate user access.
 *
 * This Source file contains the implementation of the pin class.
 * It validates the user, locking them out for 30 seconds when number of attempts reaches 2 before resetting the attempt count.
 * Otherwise, letting the user through to the next page.
 */
#include "pin.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QLineEdit>
#include <QPushButton>
#include <QInputDialog>
#include <iostream>
#include <QMessageBox>
#include <QTimer>
#include "EditProfile.h"

/**
 * @brief Construct a new PinWindow object in dark mode
 *
 * Input a pointer to the parent Widget to create a PinWindow that will validate the user before proceeding to the next page.
 *
 * @param parent The parent widget.
 */
PinWindow::PinWindow(QWidget *parent) : QWidget(parent), counter(0) {
    setWindowTitle("Pin Window");
    setFixedSize(500, 400);

    // Set a dark background for the entire window
    setStyleSheet("background-color: #121212; color: #ffffff;");

    QVBoxLayout *layout = new QVBoxLayout(this);

    // Create the main frame with a dark background
    QFrame *frame = createMainFrame();
    layout->addWidget(frame);
}

/**
 * @brief Constructs the main frame of the window in dark mode.
 *
 * Sets the layout and design of the overall window, calls two methods to create the top and middle sections individually.
 *
 * @return QFrame* The created main frame.
 */
QFrame* PinWindow::createMainFrame() {
    QFrame *frame = new QFrame(this);
    frame->setFrameShape(QFrame::Box);
    frame->setFrameShadow(QFrame::Raised);
    frame->setLineWidth(3);
    // Use a dark background and border for the frame
    frame->setStyleSheet("background-color: #1e1e1e; border: 1px solid #333333;");

    QVBoxLayout *frameLayout = new QVBoxLayout(frame);
    frameLayout->addWidget(createTopSection());
    frameLayout->addWidget(createMiddleSection());

    return frame;
}

/**
 * @brief Constructs the top section with a warning message in dark mode.
 *
 * Constructs the top section of the main frame with a warning message that indicates that
 * the user must enter the correct password to proceed.
 *
 * @return QFrame* The created top section.
 */
QFrame* PinWindow::createTopSection() {
    QFrame *topSection = new QFrame();
    topSection->setFrameShape(QFrame::Box);
    // Dark background for the top section
    topSection->setStyleSheet("background-color: #2c2c2c;");

    QLabel *warningLabel = new QLabel("WARNING", topSection);
    warningLabel->setAlignment(Qt::AlignCenter);
    // Use a bright color for warning text
    warningLabel->setStyleSheet("color: #ff5555; font-size: 20px; font-weight: bold;");

    QLabel *topLabel = new QLabel("This is a protected area, please enter the password.", topSection);
    topLabel->setAlignment(Qt::AlignCenter);
    topLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #ffffff;");

    QVBoxLayout *topLayout = new QVBoxLayout(topSection);
    topLayout->addWidget(warningLabel);
    topLayout->addWidget(topLabel);

    return topSection;
}

/**
 * @brief Constructs the middle section containing the input field and button in dark mode.
 *
 * Construct the middle section of the main frame with an input field for the user to input
 * their attempt and a button to submit the user's attempt.
 *
 * @return QFrame* The created middle section.
 */
QFrame* PinWindow::createMiddleSection() {
    QFrame *middleSection = new QFrame();
    middleSection->setFrameShape(QFrame::Box);
    // Dark background for the middle section
    middleSection->setStyleSheet("background-color: #2c2c2c;");

    QVBoxLayout *middleLayout = new QVBoxLayout(middleSection);

    // Create the input field with dark styling
    QLineEdit *textbox = new QLineEdit(middleSection);
    textbox->setPlaceholderText("Enter the pin");
    textbox->setFixedWidth(100);
    textbox->setStyleSheet("background-color: #3a3a3a; color: #ffffff; border: 1px solid #555555;");

    // Create the enter button with dark styling
    QPushButton *enterButton = new QPushButton("Enter", middleSection);
    enterButton->setFixedSize(100, 50);
    enterButton->setStyleSheet("background-color: #555555; color: #ffffff; border: none;");

    middleLayout->addWidget(textbox, 0, Qt::AlignCenter);
    middleLayout->addWidget(enterButton, 0, Qt::AlignCenter);

    // Connect signals for both the button click and return key
    connect(enterButton, &QPushButton::clicked, this, [this, textbox, enterButton]() {
        validatePassword(textbox->text(), enterButton);
    });
    connect(textbox, &QLineEdit::returnPressed, this, [this, textbox, enterButton]() {
        validatePassword(textbox->text(), enterButton);
    });

    return middleSection;
}

/**
 * @brief Validates if the inputted password is correct.
 *
 * Validates if the inputted password is correct, upon successful verification, the user is sent to the next screen
 * otherwise the attempt counter increases and once it reaches 2, the user is locked out of any attempts for 30 sec.
 * After the countdown is over, the attempt counter resets and the user is free to make more attempts.
 *
 * @param input The user's password attempt.
 * @param enterButton The button used to trigger validation.
 */
void PinWindow::validatePassword(const QString &input, QPushButton *enterButton) {
    QString correctPassword = "1234";  // Replace with your actual password

    if (counter >= 2) {
        enterButton->setEnabled(false);

        // Setup a 30-second lockout
        countdown = 30;
        timer = new QTimer(this);

        // Create and style the lockout popup for dark mode
        popup = new QMessageBox(this);
        popup->setWindowTitle("Locked Out");
        popup->setText("Too many incorrect attempts.\n" +
                       QString("Time remaining: %1 seconds").arg(countdown));
        popup->setStandardButtons(QMessageBox::NoButton);
        popup->setStyleSheet("background-color: #2c2c2c; color: #ffffff;");

        timer->start(1000);  // Update every second
        connect(timer, &QTimer::timeout, this, &PinWindow::updateCountdown);

        popup->show();
        enterButton->setEnabled(true);

        return;
    }

    // Validate the password
    if (input == correctPassword) {
       close();
       EditProfile *editProfileWin = new EditProfile();
       editProfileWin->show();
       counter = 0;
    } else {
        counter++;
        QMessageBox::warning(this, "Access Denied", "Incorrect password! Try again.");
        std::cout << "Failed Attempts: " << counter << std::endl;
    }
}

/**
 * @brief Updates the countdown timer and popup text during lockout.
 *
 * Every time timer emits a timeout() signal, the countdown decrements and the text in the message box updates accordingly.
 * Once the countdown reaches 0, the timer stops and gets deleted, the popup gets deleted, and the attempt counter restarts.
 */
void PinWindow::updateCountdown() {
    countdown--;
    popup->setText("Too many incorrect attempts.\n" +
                   QString("Time remaining: %1 seconds").arg(countdown));

    if (countdown <= 0) {
        timer->stop();
        delete popup;
        counter = 0;
    }
}