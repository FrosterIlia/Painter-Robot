#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>
#define CVUI_IMPLEMENTATION
#include "cvui.h"
#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <thread>
#include <atomic>
#include <mutex>

#include <wiringPi.h>
#include <wiringSerial.h>
#include "pigpio.h"

#include <unistd.h>

#include "EasyButton.h"

#define BUTTON_1 9
#define BUTTON_2 11


#define BUFFER_SIZE 1024

#define SERVO_PIN 4
#define CLOSED_POSITION 2100 //2100
#define OPEN_POSITION 950 //1100
#define OPEN_TIME 300


#define PINK_LED_PIN 22
#define GREEN_LED_PIN 27
#define WHITE_LED_PIN 10
#define BLUE_LED_PIN 17

const int PORT = 8080;
std::atomic<bool> server_running(false);

void drop_ball();
void sort_balls_auto();
void sort_balls_manual();
void handle_client(int client_socket, sockaddr_in client_address);
void run_server();
void sendMat(int clientsock, cv::Mat &frame);

std::mutex mtx;


bool moving = false;

cv::VideoCapture cap(0);

cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_5X5_250);

cv::Mat raw_frame, hsv_frame, pink_frame, green_frame, white_frame, blue_frame, final_frame;

int balls_numbers[4] = {0, 0, 0, 0};

int lowerTest = 0;
int upperTest = 30;

int lowerPink = 222;
int upperPink = 255;

int lowerGreen = 58;
int upperGreen = 86;

int lowerBlue = 125;
int upperBlue = 160;

int pink_counter = 0;
int green_counter = 0;
int white_counter = 0;
int blue_counter = 0;

int empty_counter = 0;

int serial;

bool manual = true;

EasyButton button1(BUTTON_1);
EasyButton button2(BUTTON_2);

int main() {
    

    if (gpioInitialise() < 0)
    {
    return 1;
    }

    if (!cap.isOpened()) {
        std::cerr << "Error: Could not open camera!" << std::endl;
        return -1;
    }
    

    if ((serial = serialOpen ("/dev/serial0", 115200)) < 0)
    {
        std::cout << "Unable to open serial device: \n";
        return -1 ;
    }

    button1.init();
    button2.init();


    gpioSetMode(SERVO_PIN, PI_OUTPUT);

    gpioSetMode(PINK_LED_PIN, PI_OUTPUT);
    gpioSetMode(GREEN_LED_PIN, PI_OUTPUT);
    gpioSetMode(WHITE_LED_PIN, PI_OUTPUT);
    gpioSetMode(BLUE_LED_PIN, PI_OUTPUT);

    gpioSetMode(BUTTON_1, PI_INPUT);
    gpioSetMode(BUTTON_2, PI_INPUT);

  

    cap.set(CV_CAP_PROP_FRAME_WIDTH, 640);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, 480);

    // gpioServo(SERVO_PIN, CLOSED_POSITION); // close servo

    // cv::namedWindow("Raspberry Pi Camera", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("Pink Detection", cv::WINDOW_AUTOSIZE);

    cvui::init("Pink Detection");

    std::thread server_thread(run_server);
    server_thread.detach();

    while (true) {

        button1.tick();
        button2.tick();
        
        if (!manual) sort_balls_auto();
        else sort_balls_manual();


        cvui::window(raw_frame, 0, 0, 200, 200, "LOL");

        cvui::checkbox(raw_frame, 10, 40, "Manual", &manual);

        if (cvui::button(raw_frame, 10, 80, "Pink") && !moving && manual){
            std::cout << "pink" << std::endl;
            balls_numbers[1]++;
            serialPuts(serial, "m1");
            serialFlush(serial);
            moving = true;

            
        }

        if (cvui::button(raw_frame, 80, 80, "Green") && !moving && manual){
            balls_numbers[2]++;
            std::cout << "green" << std::endl;
            serialPuts(serial, "m2");
            serialFlush(serial);
            moving = true;
        }

        if (cvui::button(raw_frame, 80, 120, "VIBRATE!!!") && !moving && manual){
            std::cout << "VIBRATE!!" << std::endl;
            serialPuts(serial, "m4");
            serialFlush(serial);
            moving = true;
        }

        if (button1.isHolded() && !moving){
            manual = !manual;
        }

        if (button1.isClick()){
            std::cout << "Button1: " << std::endl;
       }

       if (button2.isClick()){
        std::cout << "Button2: " << std::endl;
       }
        

        cvui::update();

        // cvui::trackbar(raw_frame, 10, 20, 580, &lowerTest, 0, 255);
        // cvui::trackbar(raw_frame, 10, 60, 580, &upperTest, 0, 255);

        cv::imshow("Pink Detection", raw_frame);

        if (cv::waitKey(1) == 27) { // 'ESC' key has ASCII value 27
            std::cout << "Exiting..." << std::endl;
            break;
        }
    }
    serialClose(serial);
    gpioTerminate();

    server_running = false;

    cap.release();
    cv::destroyAllWindows();

    return 0;
}

void drop_ball(){
    std::cout << "DROP!!" << std::endl;
    moving = false;
    gpioServo(SERVO_PIN, OPEN_POSITION);
    usleep(OPEN_TIME * 1000);
    gpioServo(SERVO_PIN, CLOSED_POSITION);
    gpioWrite(PINK_LED_PIN, 0);
    gpioWrite(GREEN_LED_PIN, 0);
    gpioWrite(WHITE_LED_PIN, 0);
    gpioWrite(BLUE_LED_PIN, 0);
}

void sort_balls_auto(){

        cap >> raw_frame;

        cv::cvtColor(raw_frame, hsv_frame, cv::COLOR_BGR2HSV_FULL);

        std::vector<int> ids;
        std::vector<std::vector<cv::Point2f>> corners;
        cv::aruco::detectMarkers(raw_frame, dictionary, corners, ids);


        cv::Scalar test_lower_range = cv::Scalar(lowerTest, 100, 100);
        cv::Scalar test_upper_range = cv::Scalar(upperTest, 255, 255);

        cv::Scalar white_lower_range = cv::Scalar(0,0,168);
        cv::Scalar white_upper_range = cv::Scalar(172,255,255);


        cv::Scalar blue_lower_range = cv::Scalar(lowerBlue, 50, 50);
        cv::Scalar blue_upper_range = cv::Scalar(upperBlue, 255, 255);
        
        
        cv::Scalar pink_lower_range = cv::Scalar(lowerPink, 100, 100);
        cv::Scalar pink_upper_range = cv::Scalar(upperPink, 255, 255);

        cv::Scalar green_lower_range = cv::Scalar(lowerGreen, 100, 100);
        cv::Scalar green_upper_range = cv::Scalar(upperGreen, 255, 255);


        cv::inRange(hsv_frame, test_lower_range, test_upper_range, final_frame);
        cv::inRange(hsv_frame, pink_lower_range, pink_upper_range, pink_frame);
        cv::inRange(hsv_frame, green_lower_range, green_upper_range, green_frame);
        cv::inRange(hsv_frame, white_lower_range, white_upper_range, white_frame);
        cv::inRange(hsv_frame, blue_lower_range, blue_upper_range, blue_frame);

        if (!ids.empty()) {
            // cv::aruco::drawDetectedMarkers(raw_frame, corners, ids);

            for (int i = 0; i < ids.size(); i++) {
                std::cout << "Detected ArUco marker with ID: " << ids[i] << std::endl;
                std::cout << "NOTHING" << std::endl;
                pink_counter = 0;
                green_counter = 0;
                white_counter = 0;
                blue_counter = 0;
                empty_counter++;
            }

            
        }
        else{
            // std::cout << "PIZDA" << std::endl;
            int pixels_pink = cv::countNonZero(pink_frame);
            int pixels_green = cv::countNonZero(green_frame);
            int pixels_white = cv::countNonZero(white_frame);
            int pixels_blue = cv::countNonZero(blue_frame);
            // std::cout << "pink: " << pixels_pink << " green: " << pixels_green << std::endl;
            // if (pixels_pink > pixels_green && pixels_pink >= 8000 && !moving){
            if (std::max({pixels_pink, pixels_white, pixels_green, pixels_blue}) == pixels_pink && pixels_pink >= 8000 && !moving){
                std::cout << "PINK" << std::endl;
                green_counter = 0;
                blue_counter = 0;
                white_counter = 0;
                pink_counter++;
                empty_counter = 0;
            }
            // else if(pixels_green > pixels_pink && pixels_green >= 8000 && !moving){
            else if (std::max({pixels_pink, pixels_white, pixels_green, pixels_blue}) == pixels_green && pixels_green >= 8000 && !moving){
                std::cout << "GREEN" << std::endl;
                pink_counter = 0;
                white_counter = 0;
                blue_counter = 0;
                green_counter++;
                empty_counter = 0;
            } 
            // else if(pixels_white >= 8000 && pixels_blue < 8000 && !moving){
            else if (std::max({pixels_pink, pixels_white, pixels_green, pixels_blue}) == pixels_white && pixels_white  >= 8000 && !moving){
                std::cout << "WHITEEEE" << std::endl;
                
                pink_counter = 0;
                green_counter = 0;
                blue_counter = 0;
                white_counter++;
            } 
            // else if(pixels_blue > pixels_white && pixels_blue >= 8000 && !moving){
            else if (std::max({pixels_pink, pixels_white, pixels_green, pixels_blue}) == pixels_blue && pixels_blue >= 8000 && !moving){
                std::cout << "BLUE" << std::endl;
                
                pink_counter = 0;
                green_counter = 0;
                white_counter = 0;
                blue_counter++;
                empty_counter = 0;
            } 
            else{
                pink_counter = 0;
                green_counter = 0;
                white_counter = 0;
                blue_counter = 0;
                std::cout << "NOTHING" << std::endl;
            } 

            if (pink_counter >= 30 && !moving){
                balls_numbers[1]++;
                serialPuts(serial, "m1");
                serialFlush(serial);
                moving = true;
                pink_counter = 0;
                gpioWrite(PINK_LED_PIN, 1);
            }
            else if (green_counter >= 30 && !moving){
                balls_numbers[2]++;
                serialPuts(serial, "m2");
                serialFlush(serial);
                moving = true;
                green_counter = 0;
                gpioWrite(GREEN_LED_PIN, 1);
            }
            else if (white_counter >= 30 && !moving){
                balls_numbers[0]++;
                serialPuts(serial, "m0");
                serialFlush(serial);
                moving = true;
                white_counter = 0;
                gpioWrite(WHITE_LED_PIN, 1);
            }
            else if (blue_counter >= 30 && !moving){
                balls_numbers[3]++;
                serialPuts(serial, "m3");
                serialFlush(serial);
                moving = true;
                blue_counter = 0;
                gpioWrite(BLUE_LED_PIN, 1);
            }
            else if (empty_counter >= 30 && !moving){
                std::cout << "vibrate" << std::endl;
                serialPuts(serial, "m4");
                serialFlush(serial);
                moving = true;
                empty_counter = 0;
            }
        }
        
        char buffer[BUFFER_SIZE] = "";
        int index = 0;
        // serialFlush(serial);
        while (true) {
            if (serialDataAvail(serial) && index < BUFFER_SIZE - 1){
                std::cout << "availbale" << std::endl;
                char c = serialGetchar(serial);
                if (c == '\n') { 
                    buffer[index] = '\0'; 
                    std::cout << "Received: " << buffer << std::endl;
                    index = 0; 
                    break;
                }
                else {
                    buffer[index++] = c; 
                    // if (index >= BUFFER_SIZE - 1) {
                    //     index = BUFFER_SIZE - 1;
                    // }
                }
            }
            else {
                if (strlen(buffer) > 0) std::cout << "Received: " << buffer << std::endl;
                
                break;
            }
            
        }
        buffer[strcspn(buffer, "\r\n")] = '\0';
        if (strcmp(buffer, "done") == 0){
            std::cout << "buffer == done" << buffer << std::endl;
            drop_ball();
        }
        else if (strcmp(buffer, "vibrated") == 0){
            std::cout << "buffer == vibrated" << buffer << std::endl;
            moving = false;
        }
        
        
        
        // cv::imshow("Final Detection", raw_frame);
        // cv::imshow("White Detection", white_frame);
        

        // cv::imshow("Raspberry Pi Camera", raw_frame);
    }

void sort_balls_manual(){
    cap >> raw_frame;

    bool is_pink = 0;
    bool is_green = 0;

    


    if ((cvui::button(raw_frame, 10, 80, "Pink") || button1.isClick()) && !moving){
        std::cout << "pink" << std::endl;
        balls_numbers[1]++;
        serialPuts(serial, "m1");
        serialFlush(serial);
        gpioWrite(PINK_LED_PIN, 1);
        moving = true;
    }

    if ((cvui::button(raw_frame, 80, 80, "Green") || button2.isClick()) && !moving){
        std::cout << "green" << std::endl;
        balls_numbers[2]++;
        serialPuts(serial, "m2");
        serialFlush(serial);
        gpioWrite(GREEN_LED_PIN, 1);
        moving = true;
    }

    char buffer[BUFFER_SIZE] = "";
        int index = 0;
        // serialFlush(serial);
        while (true) {
            if (serialDataAvail(serial) && index < BUFFER_SIZE - 1){
                std::cout << "availbale" << std::endl;
                char c = serialGetchar(serial);
                if (c == '\n') { 
                    buffer[index] = '\0'; 
                    std::cout << "Received: " << buffer << std::endl;
                    index = 0; 
                    break;
                }
                else {
                    buffer[index++] = c; 
                    // if (index >= BUFFER_SIZE - 1) {
                    //     index = BUFFER_SIZE - 1;
                    // }
                }
            }
            else {
                if (strlen(buffer) > 0) std::cout << "Received: " << buffer << std::endl;
                
                break;
            }
            
        }
        buffer[strcspn(buffer, "\r\n")] = '\0';
        if (strcmp(buffer, "done") == 0){
            std::cout << "buffer == done" << buffer << std::endl;
            drop_ball();
        }

        else if (strcmp(buffer, "vibrated") == 0){
            std::cout << "buffer == vibrated" << buffer << std::endl;
            moving = false;
        }
        
}

void handle_client(int client_socket, sockaddr_in client_address) {
    char buffer[BUFFER_SIZE] = {0};
    std::cout << "Client connected: " << inet_ntoa(client_address.sin_addr) 
              << ":" << ntohs(client_address.sin_port) << std::endl;

    while (server_running) {
        memset(buffer, 0, BUFFER_SIZE);
        int valread = read(client_socket, buffer, BUFFER_SIZE);
        
        if (valread <= 0) {
            std::cout << "Client disconnected: " << inet_ntoa(client_address.sin_addr) 
                     << ":" << ntohs(client_address.sin_port) << std::endl;
            break;
        }

        std::cout << "Received from " << inet_ntoa(client_address.sin_addr) 
                  << ": " << buffer << std::endl;

        if (buffer[0] == '$'){
            std::cout << "got $" << std::endl;
            char* new_buffer = buffer + 1;

            std::cout << *new_buffer << std::endl;
            if (!strcmp(new_buffer, "green")){
                std::cout << "green" << std::endl;
                mtx.lock();
                balls_numbers[2]++;
                serialPuts(serial, "m2");
                serialFlush(serial);
                moving = true;
                mtx.unlock();
            }

            if (!strcmp(new_buffer, "pink")){
                std::cout << "pink" << std::endl;
                mtx.lock();
                balls_numbers[1]++;
                std::cout << balls_numbers[1] << std::endl;
                serialPuts(serial, "m1");
                serialFlush(serial);
                moving = true;
                mtx.unlock();
            }

            if (!strcmp(new_buffer, "manual")){
                std::cout << "manual" << std::endl;
                mtx.lock();
                manual = true;
                mtx.unlock();
            }

            if (!strcmp(new_buffer, "auto")){
                std::cout << "auto" << std::endl;
                mtx.lock();
                manual = false;
                mtx.unlock();
            }

            if (!strcmp(new_buffer, "balls")){
                std::cout << "balls" << std::endl;
                mtx.lock();
                std::string balls_string = "white: " + std::to_string(balls_numbers[0]) + " pink: " + std::to_string(balls_numbers[1]) + " green: " + std::to_string(balls_numbers[2]) + " blue: " + std::to_string(balls_numbers[3]);
                send(client_socket, balls_string.c_str(), balls_string.length(), 0);
                mtx.unlock();
            }

            if (!strcmp(new_buffer, "picture")){
                std::cout << "picture" << std::endl;
                mtx.lock();
                // send(client_socket, "$", 1, 0);
                sendMat(client_socket, raw_frame);
                mtx.unlock();
            }
        }
        

        // Send response
        std::string response = "$Success";
        send(client_socket, response.c_str(), response.length(), 0);
    }

    close(client_socket);
}

void run_server() {
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Create socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        return;
    }

    // Forcefully attach socket to the port
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        close(server_fd);
        return;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the socket to the port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        return;
    }

    // Start listening
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        close(server_fd);
        return;
    }

    std::cout << "Server listening on port " << PORT << std::endl;
    server_running = true;

    std::vector<std::thread> client_threads;

    while (server_running) {
        int new_socket;
        struct sockaddr_in client_address;
        socklen_t client_addrlen = sizeof(client_address);

        // Accept incoming connection with timeout to allow checking server_running
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(server_fd, &readfds);
        
        struct timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        int activity = select(server_fd + 1, &readfds, nullptr, nullptr, &timeout);
        
        if (activity < 0 && errno != EINTR) {
            perror("select error");
            continue;
        }

        if (activity > 0 && FD_ISSET(server_fd, &readfds)) {
            if ((new_socket = accept(server_fd, (struct sockaddr *)&client_address, &client_addrlen)) < 0) {
                perror("accept");
                continue;
            }

            // Create a new thread for each client
            client_threads.emplace_back(handle_client, new_socket, client_address);
        }
    }

    // Wait for all client threads to finish
    for (auto& thread : client_threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    close(server_fd);
    std::cout << "Server stopped" << std::endl;
}

void sendMat(int clientsock, cv::Mat &frame) {
    std::vector<uchar> image_buffer;
    std::vector<int> compression_params = {
        cv::IMWRITE_JPEG_QUALITY, 80, // Default quality (0-100)
        cv::IMWRITE_JPEG_PROGRESSIVE, 1, // Enable progressive JPEG
        cv::IMWRITE_JPEG_OPTIMIZE, 1 // Enable optimization
    };

    if (frame.empty()) {
        std::cerr << "No image available to send" << std::endl;
        return;
    }

    // Compress the image
    image_buffer.clear();
    if (frame.empty() == false)
    {
    // Compress image to reduce size
    cv::imencode("image.jpg", frame, image_buffer, compression_params);
    }

                // Send image
    send(clientsock, reinterpret_cast<char*>(&image_buffer[0]), image_buffer.size(), 0);
            

    std::cout << "Sent image: " << image_buffer.size() << " bytes" << std::endl;
}