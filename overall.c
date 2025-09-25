/*
 * PRIORITY SOCIAL MEDIA: PRIORITIZING YOUR CONNECTIONS
 * A Mini Social Media System Implementation in C
 * 
 * Team: MindIsFull
 * Course: B.tech DS-III-T022
 * 
 * This project demonstrates the practical application of data structures
 * in building a simplified social media platform with priority-based messaging.
 */

// =============================================================================
// HEADER FILE: common.h
// Common definitions and includes for all modules
// =============================================================================

#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Constants
#define MAX_USERNAME 50
#define MAX_PASSWORD 50
#define MAX_POST_CONTENT 500
#define MAX_MESSAGE_CONTENT 300
#define MAX_FILENAME 100
#define MAX_USERS 1000

// User structure
typedef struct User {
    int user_id;
    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];
    time_t created_at;
    struct User* next;
} User;

// Post structure
typedef struct Post {
    int post_id;
    int author_id;
    char author_name[MAX_USERNAME];
    char content[MAX_POST_CONTENT];
    time_t created_at;
    int priority; // Higher for close friends
    struct Post* next;
} Post;

// Message structure
typedef struct Message {
    int message_id;
    int sender_id;
    int receiver_id;
    char sender_name[MAX_USERNAME];
    char content[MAX_MESSAGE_CONTENT];
    time_t timestamp;
    int priority; // 1 for close friends, 0 for regular
    struct Message* next;
} Message;

// Follow relationship structure
typedef struct Follow {
    int follower_id;
    int following_id;
    struct Follow* next;
} Follow;

// Close friend structure
typedef struct CloseFriend {
    int user_id;
    int friend_id;
    struct CloseFriend* next;
} CloseFriend;

// Notification structure
typedef struct Notification {
    int notif_id;
    int user_id;
    char content[MAX_MESSAGE_CONTENT];
    time_t timestamp;
    int priority;
    int is_read;
    struct Notification* next;
} Notification;

// Global variables
extern User* users_head;
extern Post* posts_head;
extern Message* messages_head;
extern Follow* follows_head;
extern CloseFriend* close_friends_head;
extern Notification* notifications_head;
extern User* current_user;
extern int next_user_id;
extern int next_post_id;
extern int next_message_id;
extern int next_notif_id;

// Function prototypes for all modules
// User module
int register_user(char* username, char* password);
User* login_user(char* username, char* password);
void logout_user();
void display_user_profile(int user_id);
User* find_user_by_id(int user_id);
User* find_user_by_username(char* username);

// Post module
int create_post(char* content);
void display_feed();
void display_user_posts(int user_id);
int get_user_priority(int user_id);

// Follow module
int follow_user(int user_id);
int unfollow_user(int user_id);
void display_followers(int user_id);
void display_following(int user_id);
int is_following(int follower_id, int following_id);

// Message module
int send_message(int receiver_id, char* content);
void display_messages(int user_id);
void display_conversation(int other_user_id);

// Close friends module
int add_close_friend(int friend_id);
int remove_close_friend(int friend_id);
void display_close_friends();
int is_close_friend(int user_id, int friend_id);

// Notification module
void add_notification(int user_id, char* content, int priority);
void display_notifications();
void mark_notification_read(int notif_id);

// File handling
void save_data();
void load_data();

// Utility functions
void clear_screen();
void pause_screen();
int get_int_input();
void get_string_input(char* buffer, int max_len);

#endif

// =============================================================================
// SOURCE FILE: user.c
// User Authentication Module - Uses Linked List
// =============================================================================



User* users_head = NULL;
User* current_user = NULL;
int next_user_id = 1;

int register_user(char* username, char* password) {
    // Check if username already exists
    User* temp = users_head;
    while (temp != NULL) {
        if (strcmp(temp->username, username) == 0) {
            return 0; // Username already exists
        }
        temp = temp->next;
    }
    
    // Create new user
    User* new_user = (User*)malloc(sizeof(User));
    if (new_user == NULL) {
        printf("Memory allocation failed!\n");
        return 0;
    }
    
    new_user->user_id = next_user_id++;
    strcpy(new_user->username, username);
    strcpy(new_user->password, password);
    new_user->created_at = time(NULL);
    new_user->next = users_head;
    users_head = new_user;
    
    printf("User registered successfully! User ID: %d\n", new_user->user_id);
    return 1;
}

User* login_user(char* username, char* password) {
    User* temp = users_head;
    while (temp != NULL) {
        if (strcmp(temp->username, username) == 0 && 
            strcmp(temp->password, password) == 0) {
            current_user = temp;
            return temp;
        }
        temp = temp->next;
    }
    return NULL; // Login failed
}

void logout_user() {
    current_user = NULL;
    printf("Logged out successfully!\n");
}

void display_user_profile(int user_id) {
    User* user = find_user_by_id(user_id);
    if (user == NULL) {
        printf("User not found!\n");
        return;
    }
    
    printf("\n=== USER PROFILE ===\n");
    printf("User ID: %d\n", user->user_id);
    printf("Username: %s\n", user->username);
    printf("Member since: %s", ctime(&user->created_at));
    
    // Count followers and following
    int followers = 0, following = 0;
    Follow* temp = follows_head;
    while (temp != NULL) {
        if (temp->following_id == user_id) followers++;
        if (temp->follower_id == user_id) following++;
        temp = temp->next;
    }
    
    printf("Followers: %d\n", followers);
    printf("Following: %d\n", following);
    printf("==================\n");
}

User* find_user_by_id(int user_id) {
    User* temp = users_head;
    while (temp != NULL) {
        if (temp->user_id == user_id) {
            return temp;
        }
        temp = temp->next;
    }
    return NULL;
}

User* find_user_by_username(char* username) {
    User* temp = users_head;
    while (temp != NULL) {
        if (strcmp(temp->username, username) == 0) {
            return temp;
        }
        temp = temp->next;
    }
    return NULL;
}

// =============================================================================
// SOURCE FILE: post.c
// Post and Feed Module - Uses Linked List with Priority Queue concept
// =============================================================================



Post* posts_head = NULL;
int next_post_id = 1;

int create_post(char* content) {
    if (current_user == NULL) {
        printf("Please login first!\n");
        return 0;
    }
    
    Post* new_post = (Post*)malloc(sizeof(Post));
    if (new_post == NULL) {
        printf("Memory allocation failed!\n");
        return 0;
    }
    
    new_post->post_id = next_post_id++;
    new_post->author_id = current_user->user_id;
    strcpy(new_post->author_name, current_user->username);
    strcpy(new_post->content, content);
    new_post->created_at = time(NULL);
    new_post->priority = 0; // Default priority
    new_post->next = posts_head;
    posts_head = new_post;
    
    // Notify followers
    Follow* temp = follows_head;
    while (temp != NULL) {
        if (temp->following_id == current_user->user_id) {
            char notif_content[MAX_MESSAGE_CONTENT];
            sprintf(notif_content, "%s created a new post", current_user->username);
            int priority = is_close_friend(temp->follower_id, current_user->user_id) ? 1 : 0;
            add_notification(temp->follower_id, notif_content, priority);
        }
        temp = temp->next;
    }
    
    printf("Post created successfully!\n");
    return 1;
}

void display_feed() {
    if (current_user == NULL) {
        printf("Please login first!\n");
        return;
    }
    
    printf("\n=== YOUR FEED ===\n");
    
    // Create priority-based feed
    Post* temp = posts_head;
    Post* priority_posts[MAX_USERS];
    Post* regular_posts[MAX_USERS];
    int priority_count = 0, regular_count = 0;
    
    while (temp != NULL) {
        // Show posts from users you follow or your own posts
        if (temp->author_id == current_user->user_id || 
            is_following(current_user->user_id, temp->author_id)) {
            
            int priority = get_user_priority(temp->author_id);
            if (priority > 0 || temp->author_id == current_user->user_id) {
                priority_posts[priority_count++] = temp;
            } else {
                regular_posts[regular_count++] = temp;
            }
        }
        temp = temp->next;
    }
    
    // Display priority posts first
    printf("--- PRIORITY POSTS (Close Friends) ---\n");
    for (int i = 0; i < priority_count; i++) {
        printf("\n[POST ID: %d] @%s\n", priority_posts[i]->post_id, 
               priority_posts[i]->author_name);
        printf("%s\n", priority_posts[i]->content);
        printf("Posted on: %s", ctime(&priority_posts[i]->created_at));
        printf("--- ⭐ PRIORITY ---\n");
    }
    
    // Display regular posts
    printf("\n--- REGULAR POSTS ---\n");
    for (int i = 0; i < regular_count; i++) {
        printf("\n[POST ID: %d] @%s\n", regular_posts[i]->post_id, 
               regular_posts[i]->author_name);
        printf("%s\n", regular_posts[i]->content);
        printf("Posted on: %s", ctime(&regular_posts[i]->created_at));
    }
    
    if (priority_count == 0 && regular_count == 0) {
        printf("No posts to display. Follow some users to see their posts!\n");
    }
    
    printf("===============\n");
}

void display_user_posts(int user_id) {
    User* user = find_user_by_id(user_id);
    if (user == NULL) {
        printf("User not found!\n");
        return;
    }
    
    printf("\n=== POSTS BY @%s ===\n", user->username);
    
    Post* temp = posts_head;
    int count = 0;
    while (temp != NULL) {
        if (temp->author_id == user_id) {
            printf("\n[POST ID: %d]\n", temp->post_id);
            printf("%s\n", temp->content);
            printf("Posted on: %s", ctime(&temp->created_at));
            count++;
        }
        temp = temp->next;
    }
    
    if (count == 0) {
        printf("No posts found for this user.\n");
    }
    
    printf("====================\n");
}

int get_user_priority(int user_id) {
    if (current_user == NULL) return 0;
    return is_close_friend(current_user->user_id, user_id) ? 1 : 0;
}

// =============================================================================
// SOURCE FILE: follow.c
// Follow/Unfollow Module - Uses Graph (Adjacency List)
// =============================================================================


Follow* follows_head = NULL;

int follow_user(int user_id) {
    if (current_user == NULL) {
        printf("Please login first!\n");
        return 0;
    }
    
    if (user_id == current_user->user_id) {
        printf("You cannot follow yourself!\n");
        return 0;
    }
    
    if (find_user_by_id(user_id) == NULL) {
        printf("User not found!\n");
        return 0;
    }
    
    // Check if already following
    if (is_following(current_user->user_id, user_id)) {
        printf("You are already following this user!\n");
        return 0;
    }
    
    Follow* new_follow = (Follow*)malloc(sizeof(Follow));
    if (new_follow == NULL) {
        printf("Memory allocation failed!\n");
        return 0;
    }
    
    new_follow->follower_id = current_user->user_id;
    new_follow->following_id = user_id;
    new_follow->next = follows_head;
    follows_head = new_follow;
    
    // Notify the followed user
    User* followed_user = find_user_by_id(user_id);
    char notif_content[MAX_MESSAGE_CONTENT];
    sprintf(notif_content, "%s started following you", current_user->username);
    add_notification(user_id, notif_content, 0);
    
    printf("You are now following @%s!\n", followed_user->username);
    return 1;
}

int unfollow_user(int user_id) {
    if (current_user == NULL) {
        printf("Please login first!\n");
        return 0;
    }
    
    Follow* temp = follows_head;
    Follow* prev = NULL;
    
    while (temp != NULL) {
        if (temp->follower_id == current_user->user_id && 
            temp->following_id == user_id) {
            
            if (prev == NULL) {
                follows_head = temp->next;
            } else {
                prev->next = temp->next;
            }
            
            User* unfollowed_user = find_user_by_id(user_id);
            printf("You have unfollowed @%s\n", unfollowed_user->username);
            free(temp);
            return 1;
        }
        prev = temp;
        temp = temp->next;
    }
    
    printf("You are not following this user!\n");
    return 0;
}

void display_followers(int user_id) {
    User* user = find_user_by_id(user_id);
    if (user == NULL) {
        printf("User not found!\n");
        return;
    }
    
    printf("\n=== FOLLOWERS OF @%s ===\n", user->username);
    
    Follow* temp = follows_head;
    int count = 0;
    while (temp != NULL) {
        if (temp->following_id == user_id) {
            User* follower = find_user_by_id(temp->follower_id);
            if (follower != NULL) {
                printf("%d. @%s (ID: %d)\n", ++count, follower->username, follower->user_id);
            }
        }
        temp = temp->next;
    }
    
    if (count == 0) {
        printf("No followers found.\n");
    }
    
    printf("========================\n");
}

void display_following(int user_id) {
    User* user = find_user_by_id(user_id);
    if (user == NULL) {
        printf("User not found!\n");
        return;
    }
    
    printf("\n=== @%s IS FOLLOWING ===\n", user->username);
    
    Follow* temp = follows_head;
    int count = 0;
    while (temp != NULL) {
        if (temp->follower_id == user_id) {
            User* following = find_user_by_id(temp->following_id);
            if (following != NULL) {
                char status[20] = "";
                if (is_close_friend(user_id, following->user_id)) {
                    strcpy(status, " ⭐ CLOSE FRIEND");
                }
                printf("%d. @%s (ID: %d)%s\n", ++count, following->username, 
                       following->user_id, status);
            }
        }
        temp = temp->next;
    }
    
    if (count == 0) {
        printf("Not following anyone.\n");
    }
    
    printf("========================\n");
}

int is_following(int follower_id, int following_id) {
    Follow* temp = follows_head;
    while (temp != NULL) {
        if (temp->follower_id == follower_id && 
            temp->following_id == following_id) {
            return 1;
        }
        temp = temp->next;
    }
    return 0;
}

// =============================================================================
// SOURCE FILE: message.c
// Messaging Module - Uses Queue (FIFO) with Priority Queue for Close Friends
// =============================================================================



Message* messages_head = NULL;
int next_message_id = 1;

int send_message(int receiver_id, char* content) {
    if (current_user == NULL) {
        printf("Please login first!\n");
        return 0;
    }
    
    if (find_user_by_id(receiver_id) == NULL) {
        printf("Receiver not found!\n");
        return 0;
    }
    
    if (receiver_id == current_user->user_id) {
        printf("You cannot send a message to yourself!\n");
        return 0;
    }
    
    Message* new_message = (Message*)malloc(sizeof(Message));
    if (new_message == NULL) {
        printf("Memory allocation failed!\n");
        return 0;
    }
    
    new_message->message_id = next_message_id++;
    new_message->sender_id = current_user->user_id;
    new_message->receiver_id = receiver_id;
    strcpy(new_message->sender_name, current_user->username);
    strcpy(new_message->content, content);
    new_message->timestamp = time(NULL);
    new_message->priority = is_close_friend(receiver_id, current_user->user_id) ? 1 : 0;
    new_message->next = messages_head;
    messages_head = new_message;
    
    // Notify receiver
    User* receiver = find_user_by_id(receiver_id);
    char notif_content[MAX_MESSAGE_CONTENT];
    sprintf(notif_content, "New message from %s", current_user->username);
    add_notification(receiver_id, notif_content, new_message->priority);
    
    printf("Message sent to @%s!\n", receiver->username);
    return 1;
}

void display_messages(int user_id) {
    if (current_user == NULL) {
        printf("Please login first!\n");
        return;
    }
    
    printf("\n=== YOUR MESSAGES ===\n");
    
    // Separate priority and regular messages
    Message* priority_messages[MAX_USERS];
    Message* regular_messages[MAX_USERS];
    int priority_count = 0, regular_count = 0;
    
    Message* temp = messages_head;
    while (temp != NULL) {
        if (temp->receiver_id == user_id || temp->sender_id == user_id) {
            if (temp->priority == 1) {
                priority_messages[priority_count++] = temp;
            } else {
                regular_messages[regular_count++] = temp;
            }
        }
        temp = temp->next;
    }
    
    // Display priority messages first
    printf("--- PRIORITY MESSAGES (Close Friends) ---\n");
    for (int i = 0; i < priority_count; i++) {
        Message* msg = priority_messages[i];
        char direction = (msg->sender_id == user_id) ? '→' : '←';
        User* other_user = (msg->sender_id == user_id) ? 
                          find_user_by_id(msg->receiver_id) : 
                          find_user_by_id(msg->sender_id);
        
        printf("\n%c @%s [MSG ID: %d] ⭐\n", direction, 
               other_user ? other_user->username : "Unknown", msg->message_id);
        printf("%s\n", msg->content);
        printf("Time: %s", ctime(&msg->timestamp));
    }
    
    printf("\n--- REGULAR MESSAGES ---\n");
    for (int i = 0; i < regular_count; i++) {
        Message* msg = regular_messages[i];
        char direction = (msg->sender_id == user_id) ? '→' : '←';
        User* other_user = (msg->sender_id == user_id) ? 
                          find_user_by_id(msg->receiver_id) : 
                          find_user_by_id(msg->sender_id);
        
        printf("\n%c @%s [MSG ID: %d]\n", direction, 
               other_user ? other_user->username : "Unknown", msg->message_id);
        printf("%s\n", msg->content);
        printf("Time: %s", ctime(&msg->timestamp));
    }
    
    if (priority_count == 0 && regular_count == 0) {
        printf("No messages found.\n");
    }
    
    printf("====================\n");
}

void display_conversation(int other_user_id) {
    if (current_user == NULL) {
        printf("Please login first!\n");
        return;
    }
    
    User* other_user = find_user_by_id(other_user_id);
    if (other_user == NULL) {
        printf("User not found!\n");
        return;
    }
    
    printf("\n=== CONVERSATION WITH @%s ===\n", other_user->username);
    
    Message* conversation[MAX_USERS];
    int count = 0;
    
    // Collect all messages between current user and other user
    Message* temp = messages_head;
    while (temp != NULL) {
        if ((temp->sender_id == current_user->user_id && temp->receiver_id == other_user_id) ||
            (temp->sender_id == other_user_id && temp->receiver_id == current_user->user_id)) {
            conversation[count++] = temp;
        }
        temp = temp->next;
    }
    
    // Sort by timestamp (simple bubble sort for demonstration)
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (conversation[j]->timestamp > conversation[j + 1]->timestamp) {
                Message* temp_msg = conversation[j];
                conversation[j] = conversation[j + 1];
                conversation[j + 1] = temp_msg;
            }
        }
    }
    
    // Display conversation in chronological order
    for (int i = 0; i < count; i++) {
        Message* msg = conversation[i];
        char* sender_name = (msg->sender_id == current_user->user_id) ? 
                           "You" : other_user->username;
        char priority_indicator = msg->priority ? '⭐' : ' ';
        
        printf("\n%s%c: %s\n", sender_name, priority_indicator, msg->content);
        printf("   %s", ctime(&msg->timestamp));
    }
    
    if (count == 0) {
        printf("No conversation found.\n");
    }
    
    printf("===============================\n");
}

// =============================================================================
// SOURCE FILE: close_friends.c
// Close Friends Module - Uses Linked List
// =============================================================================


CloseFriend* close_friends_head = NULL;

int add_close_friend(int friend_id) {
    if (current_user == NULL) {
        printf("Please login first!\n");
        return 0;
    }
    
    if (friend_id == current_user->user_id) {
        printf("You cannot add yourself as a close friend!\n");
        return 0;
    }
    
    if (find_user_by_id(friend_id) == NULL) {
        printf("User not found!\n");
        return 0;
    }
    
    // Check if already a close friend
    if (is_close_friend(current_user->user_id, friend_id)) {
        printf("User is already in your close friends list!\n");
        return 0;
    }
    
    // Must be following the user to add as close friend
    if (!is_following(current_user->user_id, friend_id)) {
        printf("You must follow this user first to add them as a close friend!\n");
        return 0;
    }
    
    CloseFriend* new_close_friend = (CloseFriend*)malloc(sizeof(CloseFriend));
    if (new_close_friend == NULL) {
        printf("Memory allocation failed!\n");
        return 0;
    }
    
    new_close_friend->user_id = current_user->user_id;
    new_close_friend->friend_id = friend_id;
    new_close_friend->next = close_friends_head;
    close_friends_head = new_close_friend;
    
    User* friend_user = find_user_by_id(friend_id);
    printf("@%s added to your close friends list!\n", friend_user->username);
    return 1;
}

int remove_close_friend(int friend_id) {
    if (current_user == NULL) {
        printf("Please login first!\n");
        return 0;
    }
    
    CloseFriend* temp = close_friends_head;
    CloseFriend* prev = NULL;
    
    while (temp != NULL) {
        if (temp->user_id == current_user->user_id && temp->friend_id == friend_id) {
            if (prev == NULL) {
                close_friends_head = temp->next;
            } else {
                prev->next = temp->next;
            }
            
            User* friend_user = find_user_by_id(friend_id);
            printf("@%s removed from your close friends list.\n", 
                   friend_user ? friend_user->username : "Unknown");
            free(temp);
            return 1;
        }
        prev = temp;
        temp = temp->next;
    }
    
    printf("User is not in your close friends list!\n");
    return 0;
}

void display_close_friends() {
    if (current_user == NULL) {
        printf("Please login first!\n");
        return;
    }
    
    printf("\n=== YOUR CLOSE FRIENDS ===\n");
    
    CloseFriend* temp = close_friends_head;
    int count = 0;
    while (temp != NULL) {
        if (temp->user_id == current_user->user_id) {
            User* friend_user = find_user_by_id(temp->friend_id);
            if (friend_user != NULL) {
                printf("%d. ⭐ @%s (ID: %d)\n", ++count, 
                       friend_user->username, friend_user->user_id);
            }
        }
        temp = temp->next;
    }
    
    if (count == 0) {
        printf("No close friends added yet.\n");
        printf("Follow users and add them to your close friends for priority content!\n");
    }
    
    printf("=========================\n");
}

int is_close_friend(int user_id, int friend_id) {
    CloseFriend* temp = close_friends_head;
    while (temp != NULL) {
        if (temp->user_id == user_id && temp->friend_id == friend_id) {
            return 1;
        }
        temp = temp->next;
    }
    return 0;
}

// =============================================================================
// SOURCE FILE: notification.c
// Notification Module - Uses Priority Queue concept with Linked List
// =============================================================================

Notification* notifications_head = NULL;
int next_notif_id = 1;

void add_notification(int user_id, char* content, int priority) {
    Notification* new_notif = (Notification*)malloc(sizeof(Notification));
    if (new_notif == NULL) {
        printf("Memory allocation failed!\n");
        return;
    }
    
    new_notif->notif_id = next_notif_id++;
    new_notif->user_id = user_id;
    strcpy(new_notif->content, content);
    new_notif->timestamp = time(NULL);
    new_notif->priority = priority;
    new_notif->is_read = 0;
    new_notif->next = notifications_head;
    notifications_head = new_notif;
}

void display_notifications() {
    if (current_user == NULL) {
        printf("Please login first!\n");
        return;
    }
    
    printf("\n=== YOUR NOTIFICATIONS ===\n");
    
    // Separate priority and regular notifications
    Notification* priority_notifs[MAX_USERS];
    Notification* regular_notifs[MAX_USERS];
    int priority_count = 0, regular_count = 0;
    
    Notification* temp = notifications_head;
    while (temp != NULL) {
        if (temp->user_id == current_user->user_id) {
            if (temp->priority == 1) {
                priority_notifs[priority_count++] = temp;
            } else {
                regular_notifs[regular_count++] = temp;
            }
        }
        temp = temp->next;
    }
    
    // Display priority notifications first
    printf("--- PRIORITY NOTIFICATIONS ---\n");
    for (int i = 0; i < priority_count; i++) {
        Notification* notif = priority_notifs[i];
        char status = notif->is_read ? ' ' : '●';
        printf("\n%c [ID: %d] ⭐ %s\n", status, notif->notif_id, notif->content);
        printf("   Time: %s", ctime(&notif->timestamp));
    }
    
    printf("\n--- REGULAR NOTIFICATIONS ---\n");
    for (int i = 0; i < regular_count; i++) {
        Notification* notif = regular_notifs[i];
        char status = notif->is_read ? ' ' : '●';
        printf("\n%c [ID: %d] %s\n", status, notif->notif_id, notif->content);
        printf("   Time: %s", ctime(&notif->timestamp));
    }
    
    if (priority_count == 0 && regular_count == 0) {
        printf("No notifications found.\n");
    }
    
    printf("=========================\n");
}

void mark_notification_read(int notif_id) {
    if (current_user == NULL) {
        printf("Please login first!\n");
        return;
    }
    
    Notification* temp = notifications_head;
    while (temp != NULL) {
        if (temp->notif_id == notif_id && temp->user_id == current_user->user_id) {
            temp->is_read = 1;
            printf("Notification marked as read.\n");
            return;
        }
        temp = temp->next;
    }
    
    printf("Notification not found or doesn't belong to you!\n");
}

// =============================================================================
// SOURCE FILE: file_handler.c
// File Handling Module - Persistent Data Storage
// =============================================================================


void save_data() {
    FILE *file;
    
    // Save users
    file = fopen("users.dat", "w");
    if (file != NULL) {
        User* temp = users_head;
        while (temp != NULL) {
            fprintf(file, "%d|%s|%s|%ld\n", 
                    temp->user_id, temp->username, temp->password, temp->created_at);
            temp = temp->next;
        }
        fclose(file);
    }
    
    // Save posts
    file = fopen("posts.dat", "w");
    if (file != NULL) {
        Post* temp = posts_head;
        while (temp != NULL) {
            fprintf(file, "%d|%d|%s|%s|%ld|%d\n", 
                    temp->post_id, temp->author_id, temp->author_name,
                    temp->content, temp->created_at, temp->priority);
            temp = temp->next;
        }
        fclose(file);
    }
    
    // Save messages
    file = fopen("messages.dat", "w");
    if (file != NULL) {
        Message* temp = messages_head;
        while (temp != NULL) {
            fprintf(file, "%d|%d|%d|%s|%s|%ld|%d\n", 
                    temp->message_id, temp->sender_id, temp->receiver_id,
                    temp->sender_name, temp->content, temp->timestamp, temp->priority);
            temp = temp->next;
        }
        fclose(file);
    }
    
    // Save follows
    file = fopen("follows.dat", "w");
    if (file != NULL) {
        Follow* temp = follows_head;
        while (temp != NULL) {
            fprintf(file, "%d|%d\n", temp->follower_id, temp->following_id);
            temp = temp->next;
        }
        fclose(file);
    }
    
    // Save close friends
    file = fopen("close_friends.dat", "w");
    if (file != NULL) {
        CloseFriend* temp = close_friends_head;
        while (temp != NULL) {
            fprintf(file, "%d|%d\n", temp->user_id, temp->friend_id);
            temp = temp->next;
        }
        fclose(file);
    }
    
    // Save notifications
    file = fopen("notifications.dat", "w");
    if (file != NULL) {
        Notification* temp = notifications_head;
        while (temp != NULL) {
            fprintf(file, "%d|%d|%s|%ld|%d|%d\n", 
                    temp->notif_id, temp->user_id, temp->content,
                    temp->timestamp, temp->priority, temp->is_read);
            temp = temp->next;
        }
        fclose(file);
    }
    
    // Save ID counters
    file = fopen("counters.dat", "w");
    if (file != NULL) {
        fprintf(file, "%d|%d|%d|%d\n", 
                next_user_id, next_post_id, next_message_id, next_notif_id);
        fclose(file);
    }
}

void load_data() {
    FILE *file;
    char line[1000];
    
    // Load ID counters first
    file = fopen("counters.dat", "r");
    if (file != NULL) {
        if (fscanf(file, "%d|%d|%d|%d", 
                   &next_user_id, &next_post_id, &next_message_id, &next_notif_id) != 4) {
            // If read fails, set defaults
            next_user_id = 1;
            next_post_id = 1;
            next_message_id = 1;
            next_notif_id = 1;
        }
        fclose(file);
    }
    
    
    // Load users
    file = fopen("users.dat", "r");
    if (file != NULL) {
        while (fgets(line, sizeof(line), file)) {
            User* new_user = (User*)malloc(sizeof(User));
            if (new_user != NULL) {
                if (sscanf(line, "%d|%49[^|]|%49[^|]|%ld", 
                          &new_user->user_id, new_user->username, 
                          new_user->password, &new_user->created_at) == 4) {
                    new_user->next = users_head;
                    users_head = new_user;
                } else {
                    free(new_user);
                }
            }
        }
        fclose(file);
    }
    
    // Load posts
    file = fopen("posts.dat", "r");
    if (file != NULL) {
        while (fgets(line, sizeof(line), file)) {
            Post* new_post = (Post*)malloc(sizeof(Post));
            if (new_post != NULL) {
                if (sscanf(line, "%d|%d|%49[^|]|%499[^|]|%ld|%d", 
                          &new_post->post_id, &new_post->author_id, new_post->author_name,
                          new_post->content, &new_post->created_at, &new_post->priority) == 6) {
                    new_post->next = posts_head;
                    posts_head = new_post;
                } else {
                    free(new_post);
                }
            }
        }
        fclose(file);
    }
    
    // Load messages
    file = fopen("messages.dat", "r");
    if (file != NULL) {
        while (fgets(line, sizeof(line), file)) {
            Message* new_message = (Message*)malloc(sizeof(Message));
            if (new_message != NULL) {
                if (sscanf(line, "%d|%d|%d|%49[^|]|%299[^|]|%ld|%d", 
                          &new_message->message_id, &new_message->sender_id, 
                          &new_message->receiver_id, new_message->sender_name,
                          new_message->content, &new_message->timestamp, 
                          &new_message->priority) == 7) {
                    new_message->next = messages_head;
                    messages_head = new_message;
                } else {
                    free(new_message);
                }
            }
        }
        fclose(file);
    }
    
    // Load follows
    file = fopen("follows.dat", "r");
    if (file != NULL) {
        while (fgets(line, sizeof(line), file)) {
            Follow* new_follow = (Follow*)malloc(sizeof(Follow));
            if (new_follow != NULL) {
                if (sscanf(line, "%d|%d", &new_follow->follower_id, &new_follow->following_id) == 2) {
                    new_follow->next = follows_head;
                    follows_head = new_follow;
                } else {
                    free(new_follow);
                }
            }
        }
        fclose(file);
    }
    
    // Load close friends
    file = fopen("close_friends.dat", "r");
    if (file != NULL) {
        while (fgets(line, sizeof(line), file)) {
            CloseFriend* new_cf = (CloseFriend*)malloc(sizeof(CloseFriend));
            if (new_cf != NULL) {
                if (sscanf(line, "%d|%d", &new_cf->user_id, &new_cf->friend_id) == 2) {
                    new_cf->next = close_friends_head;
                    close_friends_head = new_cf;
                } else {
                    free(new_cf);
                }
            }
        }
        fclose(file);
    }
    
    // Load notifications
    file = fopen("notifications.dat", "r");
    if (file != NULL) {
        while (fgets(line, sizeof(line), file)) {
            Notification* new_notif = (Notification*)malloc(sizeof(Notification));
            if (new_notif != NULL) {
                if (sscanf(line, "%d|%d|%299[^|]|%ld|%d|%d", 
                          &new_notif->notif_id, &new_notif->user_id, new_notif->content,
                          &new_notif->timestamp, &new_notif->priority, &new_notif->is_read) == 6) {
                    new_notif->next = notifications_head;
                    notifications_head = new_notif;
                } else {
                    free(new_notif);
                }
            }
        }
        fclose(file);
    }
}

// =============================================================================
// SOURCE FILE: utils.c
// Utility Functions
// =============================================================================



void clear_screen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void pause_screen() {
    printf("\nPress Enter to continue...");
    while (getchar() != '\n');
}

int get_int_input() {
    int value;
    while (scanf("%d", &value) != 1) {
        printf("Invalid input! Please enter a number: ");
        while (getchar() != '\n'); // Clear input buffer
    }
    while (getchar() != '\n'); // Clear remaining input
    return value;
}

void get_string_input(char* buffer, int max_len) {
    if (fgets(buffer, max_len, stdin) != NULL) {
        // Remove newline character if present
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len-1] == '\n') {
            buffer[len-1] = '\0';
        }
    }
}

// =============================================================================
// SOURCE FILE: main.c
// Main Program - Menu-driven Interface
// =============================================================================


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
