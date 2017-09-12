#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    PARA_S, PARA_E, H1_S, H1_E, H2_S, H2_E, H3_S, H3_E, H4_S, H4_E, H5_S, H5_E, H6_S, H6_E,
    LIST_S, LIST_E, ITEM_S, ITEM_E, EMPH_S, EMPH_E, LINK_S, LINK_M, LINK_E,
} TagType;

char TKS[][16] = {
    "<p>", "</p>\n", "<h1>", "</h1>\n", "<h2>", "</h2>\n", "<h3>", "</h3>\n", "<h4>", "</h4>\n",
    "<h5>", "</h5\n>", "<h6>", "</h6\n>", "<ul>\n", "</ul>\n", "<li>", "</li>\n", "<em>", "</em>",
    "<a href=\"", "\">", "</a>"
};

void output_char(char c){
    putchar(c);
}

int output_tk(int token){
    printf("%s", TKS[token]);
    return token;
}

void output_str(char *str){
    printf("%s", str);
}



int _token_list[64], _cur;

void clear_tk(){
    _cur = 0;
}

int push_tk(int token){
    _token_list[_cur++] = token;
    return token;
}

int pop_tk(){
    return _token_list[--_cur];
}

int peek_tk(){
    return _token_list[_cur-1];
}



#define NONE    0
#define PARA    1

char _link_buffer[200];
int _b_cur = 0;

void buf_char(char c){
    _link_buffer[_b_cur++] = c;
}

void clear_buf(){
    _b_cur = 0;
}

void read_tokens(FILE *fp){
    char c;
    int tk, is_first_return = 0;
    int st = NONE;
    clear_tk();
    while ((c = fgetc(fp))!=EOF){
        if (c == '#'){
            if (st == NONE){
                int tagdepth = 0;
                while ((c = fgetc(fp))=='#' && tagdepth < 6)
                    tagdepth++;
                if (c == ' ')
                    push_tk(output_tk(H1_S+tagdepth*2));
            }
        }
        else if(c == '*'){
            if ((c = fgetc(fp)) == ' '){
                if (_cur == 0 || peek_tk() != LIST_S)
                    push_tk(output_tk(LIST_S));
                push_tk(output_tk(ITEM_S));
            }
        }
        else if(c == '_'){
            if ((tk = peek_tk()) == EMPH_S)
                output_tk(pop_tk() + 1);
            else
                push_tk(output_tk(EMPH_S));
        }
        else if(c == '['){
            clear_buf();
            while((c = fgetc(fp)) != ']')
                buf_char(c);
            output_tk(LINK_S);
            c = fgetc(fp);
            while((c = fgetc(fp)) != ')')
                output_char(c);
            output_tk(LINK_M);
            char *ps = _link_buffer;
            for (; ps < _link_buffer+_b_cur; ps++){
                if (*ps == '_'){
                    if (peek_tk() != EMPH_S)
                        push_tk(output_tk(EMPH_S));
                    else
                        output_tk(pop_tk() + 1);
                }
                else
                    output_char(*ps);
            }
            output_tk(LINK_E);
        }
        else if(c == ')'){

        }
        else if(c == '\n'){
            if (st == NONE){
                if (_cur == 0) continue;
                output_tk(pop_tk() + 1);
            }
            else{
                if (is_first_return == 0){
                    is_first_return = 1;
                    char tempc = fgetc(fp);
                    if (tempc == EOF) break;
                    if (tempc != '\n') output_char('\n');
                    ungetc(tempc, fp);
                    continue;
                }else{
                    output_tk(pop_tk() + 1);
                    st = NONE;
                }
            }
        }
        else{
            if (st == NONE && _cur == 0){
                push_tk(output_tk(PARA_S));
                st = PARA;
                is_first_return = 0;
            }
            output_char(c);
        }
    }
    while (_cur > 0)
        output_tk(pop_tk() + 1);
}

int main(){
    FILE *fp = fopen("test.txt", "r");
    printf("run here\n");
    read_tokens(fp);
    printf("\n\n");
    return 0;
}
