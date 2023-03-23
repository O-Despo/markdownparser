#include <stdio.h>
#include <stdlib.h>

#define READ "r"
#define WRITE "w"

struct state {
   unsigned int in_code : 1;
   unsigned int in_para : 1;
   unsigned int in_emph : 1;
   unsigned int in_ital : 1;
   unsigned int in_list : 1;
   unsigned int in_head : 1;
   unsigned short head_num;
};

int main(int argc, char *argv[]){
    void process(struct state, FILE *, FILE *);

    FILE *fi;
    FILE *fo;

    if (argc != 3){
        fprintf(stderr, "Failed please provide two file names\n");
        exit(2);
    }
    
    *argv++;
    if((fi = fopen(*argv++, READ)) == NULL){
        fprintf(stderr, "Failed to open %s\n", *argv);
        exit(2);
    }


    if((fo = fopen(*argv++, WRITE)) == NULL){
        fprintf(stderr, "Failed to open %s\n", *argv);
        exit(2);
    }
    
    struct state s = {0, 0, 0, 0, 0, 0, 0};
    process(s, fi, fo);

    fclose(fi);
    fclose(fo);
}

void toEndOfLine(struct state s, char c, FILE *ifp, FILE *ofp){
    if(c == EOF){
        return;
    }
    
    fputc(c, ofp);

    while((c = fgetc(ifp)) != EOF){

        if(c == '\n'){
            return;
        }

        fputc(c, ofp);
    }
} 


char header_start(struct state s, FILE *ifp, FILE *ofp, char c){
    int count = 1;

    while((c = fgetc(ifp)) == '#'){
        count++;
    }

    if(count > 6){
        count = 6;
    }
    
    s.head_num = count; 
    s.in_head = 1;

    fprintf(ofp, "<h%d>", count);

    return c;
}

char header_finish(struct state s, FILE *ifp, FILE *ofp, char c){
    fprintf(ofp, "</h%d>", s.head_num);
    s.head_num = 0;
    s.in_head = 0;

    return c;
}

//char emph_ital_start(struct state s, FILE *ifp, FILE *ofp, char c){
//}


void process(struct state s, FILE *ifp, FILE *ofp){
    char c;
    int newLineCount = 0;

    while((c = fgetc(ifp)) != EOF){
        if(c == '#'){
            c = header_start(s, ifp, ofp, c);
        } else if (c == '\n'){
            if(s.in_head == 1){
                header_finish(s, ifp, ofp, c);
            }
        }
    }

    return;
}
