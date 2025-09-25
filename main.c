
#include "common.h"

// Function prototypes for menu functions
void display_main_menu();
void display_user_menu();
void display_social_menu();
void display_content_menu();
void display_messaging_menu();
void display_friends_menu();

void handle_registration();
void handle_login();
void handle_user_search();
void handle_profile_view();

void handle_follow();
void handle_unfollow();
void handle_view_followers();
void handle_view_following();

void handle_create_post();
void handle_view_feed();
void handle_view_user_posts();

void handle_send_message();
void handle_view_messages();
void handle_view_conversation();

void handle_add_close_friend();
void handle_remove_close_friend();
void handle_view_close_friends();
void handle_view_notifications();

int main() {
    printf("====================================\n");
    printf("  PRIORITY SOCIAL MEDIA PLATFORM   \n");
    printf("    Prioritizing Your Connections  \n");
    printf("====================================\n");
    printf("Team: MindIsFull | Course: DS-III-T022\n");
    printf("====================================\n\n");
    
    // Load existing data
    printf("Loading data...\n");
    load_data();
    printf("Data loaded successfully!\n\n");
    
    int choice;
    
    while (1) {
        if (current_user == NULL) {
            display_main_menu();
            printf("Enter your choice: ");
            choice = get_int_input();
            
            switch (choice) {
                case 1:
                    handle_registration();
                    break;
                case 2:
                    handle_login();
                    break;
                case 3:
                    handle_user_search();
                    break;
                case 4:
                    printf("Thank you for using Priority Social Media!\n");
                    printf("Saving data...\n");
                    save_data();
                    printf("Data saved successfully. Goodbye!\n");
                    return 0;
                default:
                    printf("Invalid choice! Please try again.\n");
            }
        } else {
            printf("\n=== WELCOME @%s ===\n", current_user->username);
            printf("1. User Management\n");
            printf("2. Social Network\n");
            printf("3. Content & Posts\n");
            printf("4. Messaging\n");
            printf("5. Friends & Notifications\n");
            printf("6. Logout\n");
            printf("7. Exit\n");
            printf("\nEnter your choice: ");
            
            choice = get_int_input();
            
            switch (choice) {
                case 1:
                    display_user_menu();
                    break;
                case 2:
                    display_social_menu();
                    break;
                case 3:
                    display_content_menu();
                    break;
                case 4:
                    display_messaging_menu();
                    break;
                case 5:
                    display_friends_menu();
                    break;
                case 6:
                    logout_user();
                    break;
                case 7:
                    printf("Saving data...\n");
                    save_data();
                    printf("Data saved successfully. Goodbye!\n");
                    return 0;
                default:
                    printf("Invalid choice! Please try again.\n");
            }
        }
        
        pause_screen();
    }
    
    return 0;
}

void display_main_menu() {
    printf("\n=== MAIN MENU ===\n");
    printf("1. Register\n");
    printf("2. Login\n");
    printf("3. Search Users\n");
    printf("4. Exit\n");
}

void display_user_menu() {
    printf("\n=== USER MANAGEMENT ===\n");
    printf("1. View My Profile\n");
    printf("2. Search Users\n");
    printf("3. View User Profile\n");
    printf("4. Back to Main Menu\n");
    printf("\nEnter your choice: ");
    
    int choice = get_int_input();
    
    switch (choice) {
        case 1:
            display_user_profile(current_user->user_id);
            break;
        case 2:
            handle_user_search();
            break;
        case 3:
            handle_profile_view();
            break;
        case 4:
            return;
        default:
            printf("Invalid choice!\n");
    }
}

void display_social_menu() {
    printf("\n=== SOCIAL NETWORK ===\n");
    printf("1. Follow User\n");
    printf("2. Unfollow User\n");
    printf("3. View My Followers\n");
    printf("4. View My Following\n");
    printf("5. View User's Followers\n");
    printf("6. View User's Following\n");
    printf("7. Back to Main Menu\n");
    printf("\nEnter your choice: ");
    
    int choice = get_int_input();
    
    switch (choice) {
        case 1:
            handle_follow();
            break;
        case 2:
            handle_unfollow();
            break;
        case 3:
            display_followers(current_user->user_id);
            break;
        case 4:
            display_following(current_user->user_id);
            break;
        case 5:
            handle_view_followers();
            break;
        case 6:
            handle_view_following();
            break;
        case 7:
            return;
        default:
            printf("Invalid choice!\n");
    }
}

void display_content_menu() {
    printf("\n=== CONTENT & POSTS ===\n");
    printf("1. Create Post\n");
    printf("2. View My Feed\n");
    printf("3. View My Posts\n");
    printf("4. View User's Posts\n");
    printf("5. Back to Main Menu\n");
    printf("\nEnter your choice: ");
    
    int choice = get_int_input();
    
    switch (choice) {
        case 1:
            handle_create_post();
            break;
        case 2:
            display_feed();
            break;
        case 3:
            display_user_posts(current_user->user_id);
            break;
        case 4:
            handle_view_user_posts();
            break;
        case 5:
            return;
        default:
            printf("Invalid choice!\n");
    }
}

void display_messaging_menu() {
    printf("\n=== MESSAGING ===\n");
    printf("1. Send Message\n");
    printf("2. View All Messages\n");
    printf("3. View Conversation\n");
    printf("4. Back to Main Menu\n");
    printf("\nEnter your choice: ");
    
    int choice = get_int_input();
    
    switch (choice) {
        case 1:
            handle_send_message();
            break;
        case 2:
            display_messages(current_user->user_id);
            break;
        case 3:
            handle_view_conversation();
            break;
        case 4:
            return;
        default:
            printf("Invalid choice!\n");
    }
}

void display_friends_menu() {
    printf("\n=== FRIENDS & NOTIFICATIONS ===\n");
    printf("1. Add Close Friend\n");
    printf("2. Remove Close Friend\n");
    printf("3. View Close Friends\n");
    printf("4. View Notifications\n");
    printf("5. Mark Notification as Read\n");
    printf("6. Back to Main Menu\n");
    printf("\nEnter your choice: ");
    
    int choice = get_int_input();
    
    switch (choice) {
        case 1:
            handle_add_close_friend();
            break;
        case 2:
            handle_remove_close_friend();
            break;
        case 3:
            display_close_friends();
            break;
        case 4:
            display_notifications();
            break;
        case 5:
            printf("Enter notification ID to mark as read: ");
            int notif_id = get_int_input();
            mark_notification_read(notif_id);
            break;
        case 6:
            return;
        default:
            printf("Invalid choice!\n");
    }
}

// Menu handler functions
void handle_registration() {
    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];
    
    printf("\n=== USER REGISTRATION ===\n");
    printf("Enter username: ");
    get_string_input(username, MAX_USERNAME);
    
    printf("Enter password: ");
    get_string_input(password, MAX_PASSWORD);
    
    if (register_user(username, password)) {
        printf("Registration successful!\n");
    } else {
        printf("Registration failed! Username might already exist.\n");
    }
}

void handle_login() {
    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];
    
    printf("\n=== USER LOGIN ===\n");
    printf("Enter username: ");
    get_string_input(username, MAX_USERNAME);
    
    printf("Enter password: ");
    get_string_input(password, MAX_PASSWORD);
    
    User* user = login_user(username, password);
    if (user != NULL) {
        printf("Login successful! Welcome @%s!\n", user->username);
    } else {
        printf("Login failed! Invalid username or password.\n");
    }
}

void handle_user_search() {
    char search_term[MAX_USERNAME];
    
    printf("\n=== SEARCH USERS ===\n");
    printf("Enter username to search: ");
    get_string_input(search_term, MAX_USERNAME);
    
    printf("\nSearch Results:\n");
    printf("===============\n");
    
    User* temp = users_head;
    int count = 0;
    while (temp != NULL) {
        if (strstr(temp->username, search_term) != NULL) {
            printf("%d. @%s (ID: %d)\n", ++count, temp->username, temp->user_id);
        }
        temp = temp->next;
    }
    
    if (count == 0) {
        printf("No users found matching '%s'\n", search_term);
    }
}

void handle_profile_view() {
    printf("Enter user ID to view profile: ");
    int user_id = get_int_input();
    display_user_profile(user_id);
}

void handle_follow() {
    printf("Enter user ID to follow: ");
    int user_id = get_int_input();
    follow_user(user_id);
}

void handle_unfollow() {
    printf("Enter user ID to unfollow: ");
    int user_id = get_int_input();
    unfollow_user(user_id);
}

void handle_view_followers() {
    printf("Enter user ID to view followers: ");
    int user_id = get_int_input();
    display_followers(user_id);
}

void handle_view_following() {
    printf("Enter user ID to view following: ");
    int user_id = get_int_input();
    display_following(user_id);
}

void handle_create_post() {
    char content[MAX_POST_CONTENT];
    
    printf("\n=== CREATE POST ===\n");
    printf("Enter your post content: ");
    get_string_input(content, MAX_POST_CONTENT);
    
    create_post(content);
}

void handle_view_user_posts() {
    printf("Enter user ID to view posts: ");
    int user_id = get_int_input();
    display_user_posts(user_id);
}

void handle_send_message() {
    char content[MAX_MESSAGE_CONTENT];
    
    printf("Enter receiver user ID: ");
    int receiver_id = get_int_input();
    
    printf("Enter your message: ");
    get_string_input(content, MAX_MESSAGE_CONTENT);
    
    send_message(receiver_id, content);
}

void handle_view_conversation() {
    printf("Enter user ID to view conversation: ");
    int user_id = get_int_input();
    display_conversation(user_id);
}

void handle_add_close_friend() {
    printf("Enter user ID to add as close friend: ");
    int friend_id = get_int_input();
    add_close_friend(friend_id);
}

void handle_remove_close_friend() {
    printf("Enter user ID to remove from close friends: ");
    int friend_id = get_int_input();
    remove_close_friend(friend_id);
}

/*
 * COMPILATION INSTRUCTIONS:
 * 
 * To compile this project, save all the code in a single file (e.g., social_media.c)
 * and compile using:
 * 
 * gcc -o social_media social_media.c
 * 
 * Or for better debugging:
 * gcc -Wall -Wextra -g -o social_media social_media.c
 * 
 * Then run:
 * ./social_media (on Linux/Mac)
 * social_media.exe (on Windows)
 * 
 * DATA STRUCTURES USED:
 * 
 * 1. LINKED LISTS:
 *    - User management (users_head)
 *    - Post storage (posts_head)
 *    - Message storage (messages_head)
 *    - Follow relationships (follows_head)
 *    - Close friends (close_friends_head)
 *    - Notifications (notifications_head)
 * 
 * 2. PRIORITY QUEUE CONCEPT:
 *    - Feed display (priority posts from close friends first)
 *    - Message display (priority messages from close friends first)
 *    - Notification display (priority notifications first)
 * 
 * 3. GRAPH STRUCTURE:
 *    - Follow/Following relationships using adjacency list
 * 
 * 4. QUEUE CONCEPT (FIFO):
 *    - Message ordering within conversations
 *    - Chronological display of posts and notifications
 * 
 * FEATURES IMPLEMENTED:
 * - User registration and authentication
 * - Follow/Unfollow functionality
 * - Post creation and prioritized feed
 * - Priority-based messaging system
 * - Close friends management
 * - Priority notifications
 * - Persistent data storage using files
 * - Comprehensive menu-driven interface
 * 
 * EXAMPLE USAGE:
 * 1. Register multiple users
 * 2. Login as one user
 * 3. Follow other users
 * 4. Add some as close friends
 * 5. Create posts and send messages
 * 6. Observe priority-based content display
 */