#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct User 
{
    char username[50];
    char password[50];
    char secQuestion[100];
    char secAnswer[50];
};

struct User users[100];
int userCount = 0;

void loadUsers()
{
    FILE *file = fopen("users.txt", "r");
    if (file == NULL) 
    return; 
    while (fscanf(file, "%s", users[userCount].username) != EOF)
    {
        fscanf(file, "%s", users[userCount].password);
        fgetc(file); 
        fgets(users[userCount].secQuestion, sizeof(users[userCount].secQuestion), file);
        users[userCount].secQuestion[strcspn(users[userCount].secQuestion, "\n")] = '\0';
        fscanf(file, "%s", users[userCount].secAnswer);
        userCount++;
    }
    fclose(file);
}

void saveUser(struct User user) 
{
    FILE *file = fopen("users.txt", "a");
    if (file == NULL) 
    {
        printf("Error saving user.\n");
        return;
    }
        fprintf(file, "NAME=%s\n", user.username);
        fprintf(file, "PASSWORD=%s\n", user.password);
        fprintf(file, "SECURITY QUESTION=%s\n", user.secQuestion);
        fprintf(file, "ANSWER=%s\n\n", user.secAnswer);
    fclose(file);
}

void saveUsers() 
{
    FILE *file = fopen("users.txt", "w");
    if (file == NULL) 
    {
        printf("Error saving users!\n");
        return;
    }
    for (int i = 0; i < userCount; i++) 
    {
        fprintf(file, "NAME=%s\n", users[i].username);
        fprintf(file, "PASSWORD=%s\n", users[i].password);
        fprintf(file, "SECURITY QUESTION=%s\n", users[i].secQuestion);
        fprintf(file, "ANSWER=%s\n\n", users[i].secAnswer);
    }
    fclose(file);
}

void signUp() 
{
    struct User newUser;
    printf("\n--- Sign Up ---\n");
    printf("Enter username: ");
    scanf("%s", newUser.username);
    for (int i = 0; i < userCount; i++) 
    {
        if (strcmp(users[i].username, newUser.username) == 0) 
        {
            printf("Username already exists!\n");
            return;
        }
    }
    printf("Enter password: ");
    scanf("%s", newUser.password);
    getchar(); 
    printf("Enter a security question (e.g., Your pet's name?): ");
    fgets(newUser.secQuestion, sizeof(newUser.secQuestion), stdin);
    newUser.secQuestion[strcspn(newUser.secQuestion, "\n")] = '\0'; 
    printf("Enter the answer: ");
    scanf("%s", newUser.secAnswer);
    users[userCount++] = newUser;
    saveUser(newUser);
    printf("Sign Up successful! You can now log in.\n");
}

void login() 
{
    char username[50], password[50];
    printf("\n--- Login ---\n");
    printf("Enter username: ");
    scanf("%s", username);
    printf("Enter password: ");
    scanf("%s", password);
    for (int i = 0; i < userCount; i++) 
    {
        if (strcmp(users[i].username, username) == 0 &&
            strcmp(users[i].password, password) == 0) 
        {
            printf("Login successful! Welcome, %s.\n", username);
            return;
        }
    }
    printf("Invalid username or password.\n");
}

void recoverPassword() 
{
    char username[50], answer[100], newPassword[50];
    printf("Enter your username: ");
    scanf("%s", username);
    for (int i = 0; i < userCount; i++) 
    {
        if (strcmp(users[i].username, username) == 0)
         {
            printf("Security Question: %s\n", users[i].secQuestion);
            printf("Answer: ");
            scanf(" %[^\n]", answer); 

            if (strcmp(users[i].secAnswer, answer) == 0)
             {
                printf("Correct!\n Enter your new password: ");
                scanf("%s", newPassword);

                strcpy(users[i].password, newPassword); 
                saveUsers(); 

                printf("Password updated successfully!\n");
                return;
            } else 
            {
                printf("Incorrect answer. Cannot recover.\n");
                return;
            }
        }
    }
    printf("Username not found.\n");
}

int main() {
    int choice;
    loadUsers(); 
    while (1)
    {
        printf("\n--- Mini Social Media ---\n");
        printf("1. Sign Up\n");
        printf("2. Login\n");
        printf("3. Recover Password\n");
        printf("4. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        switch (choice) 
        {
            case 1: signUp(); break;
            case 2: login(); break;
            case 3: recoverPassword(); break;
            case 4: exit(0);
            default: printf("Invalid choice.\n");
        }
    }
    return 0;
}
