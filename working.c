#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* =========================
   Minimal Social App (C)
   Features: Register/Login/Logout, Follow/Unfollow, Create Post, View Feed
   Capacity: Up to 50 users (configurable via MAX_USERS)
   ========================= */

#define MAX_USERNAME 50
#define MAX_PASSWORD 50
#define MAX_POST_CONTENT 500
#define MAX_USERS 50

typedef struct User {
    int userid;
    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];
    time_t created_at;
    struct User* next;
} User;

typedef struct Post {
    int postid;
    int authorid;
    char authorname[MAX_USERNAME];
    char content[MAX_POST_CONTENT];
    time_t created_at;
    struct Post* next;
} Post;

typedef struct Follow {
    int followerid;
    int followingid;
    struct Follow* next;
} Follow;

/* -------- Globals -------- */

static User* users_head = NULL;
static User* current_user = NULL;
static Post* posts_head = NULL;
static Follow* follows_head = NULL;

static int next_user_id = 1;
static int next_post_id = 1;
static int total_users = 0;

/* -------- Utility -------- */

static void clear_screen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

static void pause_screen() {
    printf("\nPress Enter to continue...");
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
    if (c == EOF) {
        clearerr(stdin);
    }
}

static int get_int_input() {
    int value;
    while (scanf("%d", &value) != 1) {
        printf("Invalid input! Please enter a number: ");
        int c;
        while ((c = getchar()) != '\n' && c != EOF) {}
    }
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
    return value;
}

static void get_string_input(char* buffer, int maxlen) {
    if (fgets(buffer, maxlen, stdin) != NULL) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }
    } else {
        buffer[0] = '\0';
    }
}

/* -------- Find Helpers -------- */

static User* find_user_by_id(int userid) {
    User* t = users_head;
    while (t) {
        if (t->userid == userid) return t;
        t = t->next;
    }
    return NULL;
}

static User* find_user_by_username(const char* username) {
    User* t = users_head;
    while (t) {
        if (strcmp(t->username, username) == 0) return t;
        t = t->next;
    }
    return NULL;
}

static int is_following(int followerid, int followingid) {
    Follow* t = follows_head;
    while (t) {
        if (t->followerid == followerid && t->followingid == followingid) return 1;
        t = t->next;
    }
    return 0;
}

/* -------- Core: Auth -------- */

static int register_user(const char* username, const char* password) {
    if (strlen(username) == 0) {
        printf("Username cannot be empty!\n");
        return 0;
    }
    if (strlen(password) == 0) {
        printf("Password cannot be empty!\n");
        return 0;
    }
    if (total_users >= MAX_USERS) {
        printf("User capacity reached (%d). Registration is closed.\n", MAX_USERS);
        return 0;
    }
    if (find_user_by_username(username)) {
        printf("Username already exists!\n");
        return 0;
    }
    User* u = (User*)malloc(sizeof(User));
    if (!u) {
        printf("Memory allocation failed!\n");
        return 0;
    }
    u->userid = next_user_id++;
    strncpy(u->username, username, MAX_USERNAME - 1);
    u->username[MAX_USERNAME - 1] = '\0';
    strncpy(u->password, password, MAX_PASSWORD - 1);
    u->password[MAX_PASSWORD - 1] = '\0';
    u->created_at = time(NULL);
    u->next = users_head;
    users_head = u;
    total_users++;
    printf("User registered successfully! User ID: %d (Total users: %d/%d)\n", u->userid, total_users, MAX_USERS);
    return 1;
}

static User* login_user(const char* username, const char* password) {
    User* u = find_user_by_username(username);
    if (u && strcmp(u->password, password) == 0) {
        current_user = u;
        return u;
    }
    return NULL;
}

static void logout_user() {
    current_user = NULL;
    printf("Logged out successfully!\n");
}

static void display_user_profile(int userid) {
    User* u = find_user_by_id(userid);
    if (!u) {
        printf("User not found!\n");
        return;
    }
    printf("\n--- USER PROFILE ---\n");
    printf("User ID    : %d\n", u->userid);
    printf("Username   : %s\n", u->username);
    printf("Member since: %s", ctime(&u->created_at));

    int followers = 0, following = 0;
    Follow* f = follows_head;
    while (f) {
        if (f->followingid == userid) followers++;
        if (f->followerid == userid) following++;
        f = f->next;
    }
    printf("Followers: %d\n", followers);
    printf("Following: %d\n", following);
}

/* -------- Core: Follow -------- */

static int follow_user(int userid) {
    if (!current_user) {
        printf("Please login first!\n");
        return 0;
    }
    if (userid == current_user->userid) {
        printf("You cannot follow yourself!\n");
        return 0;
    }
    if (!find_user_by_id(userid)) {
        printf("User not found!\n");
        return 0;
    }
    if (is_following(current_user->userid, userid)) {
        printf("You are already following this user!\n");
        return 0;
    }
    Follow* f = (Follow*)malloc(sizeof(Follow));
    if (!f) {
        printf("Memory allocation failed!\n");
        return 0;
    }
    f->followerid = current_user->userid;
    f->followingid = userid;
    f->next = follows_head;
    follows_head = f;

    User* followed = find_user_by_id(userid);
    printf("You are now following %s!\n", followed ? followed->username : "(unknown)");
    return 1;
}

static int unfollow_user(int userid) {
    if (!current_user) {
        printf("Please login first!\n");
        return 0;
    }
    Follow* t = follows_head;
    Follow* prev = NULL;
    while (t) {
        if (t->followerid == current_user->userid && t->followingid == userid) {
            if (!prev) follows_head = t->next;
            else prev->next = t->next;
            User* u = find_user_by_id(userid);
            printf("You have unfollowed %s.\n", u ? u->username : "(unknown)");
            free(t);
            return 1;
        }
        prev = t;
        t = t->next;
    }
    printf("You are not following this user!\n");
    return 0;
}

static void display_followers(int userid) {
    User* u = find_user_by_id(userid);
    if (!u) {
        printf("User not found!\n");
        return;
    }
    printf("\n--- FOLLOWERS OF %s ---\n", u->username);
    Follow* t = follows_head;
    int count = 0;
    while (t) {
        if (t->followingid == userid) {
            User* f = find_user_by_id(t->followerid);
            if (f) {
                printf("%d. %s (ID %d)\n", ++count, f->username, f->userid);
            }
        }
        t = t->next;
    }
    if (count == 0) printf("No followers found.\n");
}

static void display_following(int userid) {
    User* u = find_user_by_id(userid);
    if (!u) {
        printf("User not found!\n");
        return;
    }
    printf("\n--- %s IS FOLLOWING ---\n", u->username);
    Follow* t = follows_head;
    int count = 0;
    while (t) {
        if (t->followerid == userid) {
            User* g = find_user_by_id(t->followingid);
            if (g) {
                printf("%d. %s (ID %d)\n", ++count, g->username, g->userid);
            }
        }
        t = t->next;
    }
    if (count == 0) printf("Not following anyone.\n");
}

/* -------- Core: Posts -------- */

static int create_post(const char* content) {
    if (!current_user) {
        printf("Please login first!\n");
        return 0;
    }
    if (strlen(content) == 0) {
        printf("Post content cannot be empty!\n");
        return 0;
    }
    Post* p = (Post*)malloc(sizeof(Post));
    if (!p) {
        printf("Memory allocation failed!\n");
        return 0;
    }
    p->postid = next_post_id++;
    p->authorid = current_user->userid;
    strncpy(p->authorname, current_user->username, MAX_USERNAME - 1);
    p->authorname[MAX_USERNAME - 1] = '\0';
    strncpy(p->content, content, MAX_POST_CONTENT - 1);
    p->content[MAX_POST_CONTENT - 1] = '\0';
    p->created_at = time(NULL);
    p->next = posts_head;
    posts_head = p;
    printf("Post created successfully!\n");
    return 1;
}

static void display_feed() {
    if (!current_user) {
        printf("Please login first!\n");
        return;
    }
    printf("\n--- YOUR FEED ---\n");
    Post* t = posts_head;
    int count = 0;
    while (t) {
        if (t->authorid == current_user->userid || is_following(current_user->userid, t->authorid)) {
            printf("POST ID %d | %s\n", t->postid, t->authorname);
            printf("%s\n", t->content);
            printf("Posted on: %s", ctime(&t->created_at));
            printf("-------------------------\n");
            count++;
        }
        t = t->next;
    }
    if (count == 0) {
        printf("No posts to display. Follow users or create a post!\n");
    }
}

static void display_user_posts(int userid) {
    User* u = find_user_by_id(userid);
    if (!u) {
        printf("User not found!\n");
        return;
    }
    printf("\n--- POSTS BY %s ---\n", u->username);
    Post* t = posts_head;
    int count = 0;
    while (t) {
        if (t->authorid == userid) {
            printf("POST ID %d\n", t->postid);
            printf("%s\n", t->content);
            printf("Posted on: %s", ctime(&t->created_at));
            printf("-------------------------\n");
            count++;
        }
        t = t->next;
    }
    if (count == 0) printf("No posts found for this user.\n");
}

/* -------- Memory Cleanup -------- */

static void cleanup_memory() {
    // Free users
    User* u = users_head;
    while (u) {
        User* temp = u;
        u = u->next;
        free(temp);
    }
    
    // Free posts
    Post* p = posts_head;
    while (p) {
        Post* temp = p;
        p = p->next;
        free(temp);
    }
    
    // Free follows
    Follow* f = follows_head;
    while (f) {
        Follow* temp = f;
        f = f->next;
        free(temp);
    }
}

/* -------- Menus -------- */

static void main_menu_logged_out() {
    printf("\n=== MAIN MENU ===\n");
    printf("1. Register\n");
    printf("2. Login\n");
    printf("3. Search Users\n");
    printf("4. Exit\n");
    printf("Enter your choice: ");
}

static void main_menu_logged_in() {
    printf("\n=== MENU ===\n");
    printf("1. View My Profile\n");
    printf("2. Search Users\n");
    printf("3. View User Profile\n");
    printf("4. Follow User\n");
    printf("5. Unfollow User\n");
    printf("6. View My Followers\n");
    printf("7. View My Following\n");
    printf("8. Create Post\n");
    printf("9. View My Feed\n");
    printf("10. View User Posts\n");
    printf("11. Logout\n");
    printf("12. Exit\n");
    printf("Enter your choice: ");
}

/* -------- Handlers -------- */

static void handle_registration() {
    char username[MAX_USERNAME], password[MAX_PASSWORD];
    printf("\n--- USER REGISTRATION ---\n");
    if (total_users >= MAX_USERS) {
        printf("Registration closed. Capacity %d reached.\n", MAX_USERS);
        return;
    }
    printf("Enter username: ");
    get_string_input(username, MAX_USERNAME);
    printf("Enter password: ");
    get_string_input(password, MAX_PASSWORD);
    register_user(username, password);
}

static void handle_login() {
    char username[MAX_USERNAME], password[MAX_PASSWORD];
    printf("\n--- USER LOGIN ---\n");
    printf("Enter username: ");
    get_string_input(username, MAX_USERNAME);
    printf("Enter password: ");
    get_string_input(password, MAX_PASSWORD);
    User* u = login_user(username, password);
    if (u) {
        printf("Login successful! Welcome, %s!\n", u->username);
    } else {
        printf("Login failed! Invalid username or password.\n");
    }
}

static void handle_user_search() {
    char term[MAX_USERNAME];
    printf("\n--- SEARCH USERS ---\n");
    printf("Enter username to search: ");
    get_string_input(term, MAX_USERNAME);
    printf("\nResults:\n");
    User* t = users_head;
    int count = 0;
    while (t) {
        if (strstr(t->username, term)) {
            printf("%d. %s (ID %d)\n", ++count, t->username, t->userid);
        }
        t = t->next;
    }
    if (count == 0) printf("No users found matching '%s'.\n", term);
}

static void handle_profile_view() {
    printf("Enter user ID to view profile: ");
    int id = get_int_input();
    display_user_profile(id);
}

static void handle_follow() {
    printf("Enter user ID to follow: ");
    int id = get_int_input();
    follow_user(id);
}

static void handle_unfollow() {
    printf("Enter user ID to unfollow: ");
    int id = get_int_input();
    unfollow_user(id);
}

static void handle_create_post() {
    char content[MAX_POST_CONTENT];
    printf("\n--- CREATE POST ---\n");
    printf("Enter your post content:\n");
    get_string_input(content, MAX_POST_CONTENT);
    create_post(content);
}

static void handle_view_user_posts() {
    printf("Enter user ID to view posts: ");
    int id = get_int_input();
    display_user_posts(id);
}

/* -------- Optional: Seed users for demo -------- */
static void seed_demo_users(int count) {
    if (count < 0) count = 0;
    if (count > MAX_USERS) count = MAX_USERS;
    char uname[64], pword[64];
    for (int i = 0; i < count; i++) {
        if (total_users >= MAX_USERS) break;
        snprintf(uname, sizeof(uname), "user%02d", i + 1);
        snprintf(pword, sizeof(pword), "pass%02d", i + 1);
        register_user(uname, pword);
    }
}

/* -------- Main -------- */

int main() {
    printf("\n===============================\n");
    printf("  Minimal Social Media (C)\n");
    printf("  Login/Follow/Post Demo\n");
    printf("  Capacity: %d users\n", MAX_USERS);
    printf("===============================\n");

    /* Uncomment to precreate users for testing (e.g., 10 out of 50) */
    /* seed_demo_users(10); */

    while (1) {
        if (!current_user) {
            main_menu_logged_out();
            int choice = get_int_input();
            switch (choice) {
                case 1: handle_registration(); break;
                case 2: handle_login(); break;
                case 3: handle_user_search(); break;
                case 4: 
                    printf("Goodbye!\n");
                    cleanup_memory();
                    return 0;
                default: printf("Invalid choice! Please try again.\n"); break;
            }
        } else {
            printf("\nWelcome, %s! (Users: %d/%d)\n", current_user->username, total_users, MAX_USERS);
            main_menu_logged_in();
            int choice = get_int_input();
            switch (choice) {
                case 1: display_user_profile(current_user->userid); break;
                case 2: handle_user_search(); break;
                case 3: handle_profile_view(); break;
                case 4: handle_follow(); break;
                case 5: handle_unfollow(); break;
                case 6: display_followers(current_user->userid); break;
                case 7: display_following(current_user->userid); break;
                case 8: handle_create_post(); break;
                case 9: display_feed(); break;
                case 10: handle_view_user_posts(); break;
                case 11: logout_user(); break;
                case 12: 
                    printf("Goodbye!\n");
                    cleanup_memory();
                    return 0;
                default: printf("Invalid choice! Please try again.\n"); break;
            }
        }
        pause_screen();
        clear_screen();
    }
    
    cleanup_memory();
    return 0;
}
