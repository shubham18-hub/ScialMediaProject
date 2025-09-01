#include<stdio.h>
#include<stdlib.h>
#include<string.h>


typedef struct Post{
    int postID;
    int userID;
    char imagePath[100];
    char caption[100];
    struct Post* next;
}post;

post* upload(post*);
// post* remove(post*);
// void display(post*);


int main(){
    post* Top=NULL;
    int choice;
    do{
        printf("Enter your choice: \n1.Upload Post \n2.Remove Post \n3.Display \n4.Exit\n");
        scanf("%d",&choice);
        switch(choice){
            case 1: Top=upload(Top);
                    break;
            // case 2: Top=remove(Top);
            //         break;
            // case 3: display(Top);
            //         break;
            case 4: break;
        }
    }while(choice<=3);
    return 0;
}

post* upload(post* Top){
    char ImagePath[100],Caption[100];
    post * p =NULL;int v;
    p=(post*)malloc(sizeof(post));
    if(p==NULL) {printf("MNA");}
    else{
        getchar();
        printf("Enter the image path:");
        fgets(ImagePath,99,stdin);
        printf("Enter the caption:");
        fgets(Caption,99,stdin);
        strcpy(p->imagePath,ImagePath);
        strcpy(p->caption,Caption);
        p->next=Top;
        Top=p;
    }
    return Top;
}

// post* remove(post* Top){}