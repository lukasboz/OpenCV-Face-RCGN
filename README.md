# Face Recognition System Using OpenCV

## Description
This project is a facial recognition system developed using OpenCV, designed for security applications. It allows for real-time face detection, user profile management, and permission-based access control. The system assigns permission levels—**Admin, Manager, or Employee**—to individual profiles, determining their access to conceptual "locked doors."

The **UI** integrates an OpenCV camera feed with profile management functionalities, enabling administrators to add, remove, and modify user profiles through a **secure admin panel**. Access to user management is protected by a pop-up authentication window. Admins can update user permissions, add/remove face images, and retrain the model directly from the interface.

---

## Features
- **Real-time Facial Recognition**
    - Uses **Haar Cascades** for efficient face detection.
    - Captures and recognizes faces from a live video feed.
- **User Profile Management**
    - Add, remove, and modify user profiles with face images.
    - Assign and adjust permission levels (**Admin, Manager, Employee**).
- **Admin Authentication**
    - Secure pop-up window for admin authentication before profile management.
- **Dataset Handling**
    - Add or remove face images to/from user datasets.
    - Access local directories via file explorer for easy image selection.
    - Retrain the model after dataset modifications.
- **Consistent UI Design**
    - Uniform interface across all windows.
    - Smooth navigation between camera feed, profile management, and admin panel.

---

## Installation Instructions

### Prerequisites
- **C++** (>= 17)
- **OpenCV** (>= 4.5)
- **Qt version 5 or later**

### Install Dependencies
You can install all dependencies from their respective websites linked below:
Qt: https://www.qt.io/download-dev
OpenCV: https://docs.opencv.org/4.x/d3/d52/tutorial_windows_install.html

