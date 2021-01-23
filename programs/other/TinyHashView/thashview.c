/* Copyright (C) 2019-2021 Logaev Maxim (turbocat2001), GPLv2 */

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <kos32sys1.h>
#include <string.h>
#include <cryptal/md5.h>
#include <cryptal/sha1.h>
#include <cryptal/sha256.h>
#include <clayer/dialog.h>
#include <clayer/boxlib.h>

#define TRUE 1;
#define FALSE 0;
#define MAX_HASH_LEN 65 // ���ᨬ��쭠� ����� ��ப�
#define WINDOW_W 665
#define VERSION "%s - thashview 2.6"
#define EDIT_TEXT_SIZE 0x10000000

struct kolibri_system_colors sys_color_table;

char hash_str_md5[MAX_HASH_LEN]=   "Click the 'MD5:' button to show the md5-checksum!      "; //�뢮� MD5
char hash_str_sha1[MAX_HASH_LEN]=  "Click the 'SHA1:' button to show the sha1-checksum!    "; //�뢮� SHA1
char hash_str_sha256[MAX_HASH_LEN]="Click the 'SHA256:' button to show the sha256-checksum!"; //�뢮� SHA256
char edit_box_buff[MAX_HASH_LEN]; // ����� ��� �����
char *filename; // ��� ��ࠡ��뢠����� 䠩��
char *title; // ��������� ����

enum MYCOLORS // ����
{
    GREEN = 0x067D06 | EDIT_TEXT_SIZE,
    RED   = 0xFF0000 | EDIT_TEXT_SIZE,
    BLACK = 0x000000 | EDIT_TEXT_SIZE,
    WHITE = 0xFFFFFF,
    GREY  = 0x919191
};

edit_box hash_edit_box={WINDOW_W-140,10,121,WHITE,0,0,GREY,EDIT_TEXT_SIZE, MAX_HASH_LEN-1, edit_box_buff,NULL,ed_focus}; // ������ �������� edit_box
int md5_flag=0, sha1_flag=0, sha256_flag=0; // ����� �����뢠�騥 �뫠 �� 㦥 ����⠭� ���஫쭠� �㬬� � �㭪樨 check_sum()

enum BUTTONS // ������ � ����䥩�
{
    BTN_QUIT=1,        //��室
    BTN_MD5 = 10,      //�������� md5-����஫��� �㬬�
    BTN_SHA1 = 20,     //�������� sha1-����஫��� �㬬�
    BTN_SHA256 = 30,   //�������� sha256-����஫��� �㬬�
    BTN_COPY_MD5= 11,  //�����஢��� � ����� ������
    BTN_COPY_SHA1= 21,
    BTN_COPY_SHA256=31,
    BTN_CMP=40,        //�ࠢ���� edit_box � ����஫��� �㬬�
    BTN_PASTE=50       //��⠢��� � edit_box(���� � ࠧࠡ�⪥)
};

void notify_show(char *text)
{
   start_app("/sys/@notify", text);
}

void* safe_malloc(size_t size) // �������� malloc. �����뢠�� 㢥�������� �� �訡�� � ����뢠�� �ணࠬ�� �᫨ ������ �� �뫠 �뤥����
{
    void *p=malloc(size);
    if(p==NULL)
    {
       notify_show("'Memory allocation error!' -E");
       exit(0);
    }
    else
    {
        return p;
    }
}

void global_var_init(unsigned int size)  // ���樠���������� �������� ���ᨢ�
{
  filename=safe_malloc(size);
  title=safe_malloc(size+20);
}

/* �㭪樨 �����樨 ����஫��� �㬬 */
void md5_hash(FILE* input, BYTE* hash )
{
    int input_size;
    BYTE *temp_buffer;
    temp_buffer=safe_malloc(1024);
    MD5_CTX ctx;
    md5_init(&ctx);
    while((input_size = fread(temp_buffer, 1, 1024, input)) > 0){
                md5_update(&ctx, temp_buffer, input_size);
    }
    md5_final(&ctx, hash);
    free(temp_buffer);
}

void sha1_hash(FILE* input, BYTE* hash )
{
    int input_size;
    BYTE *buffer;
    buffer=safe_malloc(1024);
    SHA1_CTX ctx;
    sha1_init(&ctx);
    while((input_size = fread(buffer, 1, 1024, input)) > 0){
                sha1_update(&ctx, buffer, input_size);
    }
    sha1_final(&ctx, hash);
    free(buffer);
}

void sha256_hash(FILE* input, BYTE* hash )
{
    int input_size;
    BYTE *buffer;
    buffer=safe_malloc(1024);
    SHA256_CTX ctx;
    sha256_init(&ctx);
    while((input_size = fread(buffer, 1, 1024, input)) > 0){
                sha256_update(&ctx, buffer, input_size);
    }
    sha256_final(&ctx, hash);
    free(buffer);
}


BYTE* check_sum(int alg) // ������㥬 ����஫�� �㬬� �ᯮ���� ���� �� �����⬮�
{
    FILE* input_file;
    BYTE *hash;
    input_file=fopen(filename,"rb");
    hash = safe_malloc(alg);
    switch (alg)
    {
        case MD5_BLOCK_SIZE :
            md5_hash(input_file, hash);
            md5_flag=1;
        break;

        case SHA1_BLOCK_SIZE :
            sha1_hash(input_file, hash);
            sha1_flag=1;
        break;

        case SHA256_BLOCK_SIZE :
            sha256_hash(input_file, hash);
            sha256_flag=1;
        break;
    }
    fclose(input_file);
    return hash;
}

void sprint_hash(BYTE *hash, char* hash_str, int hash_size) //�८��㥬 ������ ����� �� hash � ��ப� hash_str
{
    char block[3];
    memset(hash_str, 0, MAX_HASH_LEN); // ��頥� ��ப� ��� strcat
    for(int i=0; i<hash_size; i++)
    {
        sprintf(block,"%02x", hash[i]);
        strcat(hash_str,block);
    }
    free(hash);
}

void redraw_window() //���㥬 ����
{
    sprintf(title,VERSION, filename); // ��⠭�������� ��������� ����
    pos_t win_pos = get_mouse_pos(0); // ����砥� ���न���� �����
    begin_draw(); //��稭��� �ᮢ���� ����䥩� )
    sys_create_window(win_pos.x, win_pos.y, WINDOW_W, 150, title, sys_color_table.work_area, 0x14); // ������ ����.

    edit_box_draw(&hash_edit_box); // ���㥬 edit_box

    define_button(X_W(10,60), Y_H(30,20), BTN_MD5, GREEN); // ��।��塞 ������ md5
    define_button(X_W(10,60), Y_H(60,20), BTN_SHA1, GREEN);// ��।��塞 ������ sha1
    define_button(X_W(10,60), Y_H(90,20), BTN_SHA256, GREEN);// ��।��塞 ������ sha256

    draw_text_sys("MD5:", 15, 34, 0,   0x90000000 | sys_color_table.work_button_text); // ��饬 ⥪�� �� �������
    draw_text_sys("SHA1:", 15, 64, 0,  0x90000000 | sys_color_table.work_button_text);
    draw_text_sys("SHA256:", 15,94, 0, 0x90000000 | sys_color_table.work_button_text);

    draw_text_sys(hash_str_md5, 80, 34, 0, 0x90000000 | sys_color_table.work_text); // �뢮��� ����஫�� �㬬� � ����
    draw_text_sys(hash_str_sha1, 80, 64, 0, 0x90000000 | sys_color_table.work_text);
    draw_text_sys(hash_str_sha256, 80, 94, 0, 0x90000000| sys_color_table.work_text);

    define_button(X_W(610,42), Y_H(30, 20), BTN_COPY_MD5, sys_color_table.work_button); // ��।��塞 ������ ��� ����஢����
    define_button(X_W(610,42), Y_H(60, 20), BTN_COPY_SHA1, sys_color_table.work_button);
    define_button(X_W(610,42), Y_H(90, 20), BTN_COPY_SHA256, sys_color_table.work_button);

    draw_text_sys("Copy", 615, 34, 0,   0x90000000 | sys_color_table.work_button_text); // ��襬 copy �� ��� ������� ��� ����஢����
    draw_text_sys("Copy", 615, 64, 0,  0x90000000 | sys_color_table.work_button_text);
    draw_text_sys("Copy", 615, 94, 0, 0x90000000 | sys_color_table.work_button_text);

    define_button(X_W(592,60), Y_H(120,20), BTN_CMP, GREEN); // ��।��塞 ������ ��� �ࠢ����� ���⮫��� �㬬
    draw_text_sys("Compare", 595, 124 , 0,0x90000000 | sys_color_table.work_button_text); // ��襬 ⥪� �� ������.
    define_button(X_W(540, 45), Y_H(120,20), BTN_PASTE, sys_color_table.work_button); //������ ��� ��⠢�� (��ࠡ�⠥�)
    draw_text_sys("Paste", 543, 124 , 0,0x90000000 | sys_color_table.work_button_text); // ����� paste �� ������
    end_draw();
}

void paste_to_edit_buffer()    // ��⠢��� �� ����� ������
{
    char *temp_buff=NULL;
    if(kol_clip_num()>0)
    {
        temp_buff=kol_clip_get(kol_clip_num()-1);
        memset(edit_box_buff,0,MAX_HASH_LEN);
        if(DATA(int, temp_buff,0)>0 && DATA(int,temp_buff,4)==TEXT && DATA(int,temp_buff,8)==CP866)
        {
            strncpy(edit_box_buff,temp_buff+12, MAX_HASH_LEN-1);
            edit_box_set_text(&hash_edit_box,edit_box_buff);
            notify_show("'Pasted from clipboard!' -I");
            hash_edit_box.text_color = BLACK;
            user_free(temp_buff);
        }
    }
}

void copy_to_clipboard(char *text) // ����ૢ��� � ����� ������
{
    if(55!=strlen(text))
    {
        char *temp_buffer=safe_malloc(MAX_HASH_LEN+12);
        memset(temp_buffer, 0, MAX_HASH_LEN);
        DATA(char,temp_buffer,4)=TEXT;
        DATA(char,temp_buffer,8)=CP866;
        strncpy(temp_buffer+12, text, MAX_HASH_LEN-1);
        kol_clip_set(strlen(text)+12, temp_buffer);
        notify_show("'Copied to clipboard!' -I");
        free(temp_buffer);
    }
}

void print_pending_calc(char *str) // �뢮��� ᮮ�饭�� � ⮬ �� ����஫쭠� �㬬�� ��������.
{
  strcpy(str, "Please wait! Calculating checksum...                   ");
  redraw_window();
}

bool calc_and_cmp(char *hash_str_universal,int alg) // ����塞 ����஫��� �㬬� � �ࠢ������ � edit_box_buff.
{
   print_pending_calc(hash_str_universal);
   sprint_hash(check_sum(alg),hash_str_universal, alg);
   return !strcmp(edit_box_buff, hash_str_universal);
}

bool hash_compare() // ������� �㭪�� ��� �ࠢ�����
{
   int alg=strlen(edit_box_buff)/2;

        switch (alg) // �᫨ ���᫥��� ��� ���뫮
        {
        case MD5_BLOCK_SIZE:
            if(md5_flag){
                return !strcmp(edit_box_buff,hash_str_md5);
            }else{
                return calc_and_cmp(hash_str_md5,alg);
            }
        break;

        case SHA1_BLOCK_SIZE:
            if(sha1_flag){
                return !strcmp(edit_box_buff,hash_str_sha1);
            }else{
                return calc_and_cmp(hash_str_sha1,alg);
            }
        break;

        case SHA256_BLOCK_SIZE:

            if(sha256_flag){
                return !strcmp(edit_box_buff,hash_str_sha256);
            }else{
                return calc_and_cmp(hash_str_sha256,alg);
            }
        break;

        default:
            return FALSE;
        break;
        }
}

int main(int argc, char** argv)
{
    kolibri_boxlib_init(); // ����㦠�� boxlib
    if(argc<2) // �᫨ ��㬥�⮢ ���, � ����᪠�� ������ �롮� 䠩��
    {
        kolibri_dialog_init(); // ����㦠�� proc_lib(libdialog)
        open_dialog* dialog = kolibri_new_open_dialog(OPEN,0, 0, 420, 320);
        OpenDialog_init(dialog);
        OpenDialog_start(dialog); 
        if(dialog->status==SUCCESS) // �᫨ 䠩� ��࠭
        {
            global_var_init(strlen(dialog->openfile_path));
            strcpy(filename, dialog->openfile_path);  
        }
        else // �᫨ 䠩� �� ��࠭
        {
            notify_show("'No file selected!' -E");
            exit(0);
        }
        free(dialog);
    }
    else
    {
        global_var_init(strlen(argv[1]));
        strcpy(filename, argv[1]);
    }

    if(NULL==fopen(filename,"rb")) // �᫨ 䠩�� ��� ��� �� ���뢠����
    {
        notify_show("'File not found!' -E");
        exit(0);
    }

    if(GetScreenSize()/65536<WINDOW_W)
    {
        notify_show("'Low screen resolution! Program will not display correctrly!' -W");
    }

    int gui_event; // ��६����� ��� �࠭���� ᮡ���
    uint32_t pressed_button = 0; // ��� ����⮩ ������ � ����

    get_system_colors(&sys_color_table);
    hash_edit_box.shift_color=sys_color_table.work_button;

    set_event_mask(0xC0000027);// ��⠭�������� ���� ᮡ�⨩
    do // ���� ��ࠡ�⪨ ᮡ�⨩
    {
        gui_event = get_os_event(); // ����砥� ᮡ�⨥
        switch(gui_event) // ��ࠡ��뢠�� ᮡ���
        {
        case KOLIBRI_EVENT_NONE:
            break;
        case KOLIBRI_EVENT_REDRAW:
            redraw_window();
            break;
        case KOLIBRI_EVENT_MOUSE:
            edit_box_mouse(&hash_edit_box);
            break;        
        case KOLIBRI_EVENT_KEY:
            hash_edit_box.text_color = BLACK;
            edit_box_key(&hash_edit_box,get_key().val);
            break;
        case KOLIBRI_EVENT_BUTTON: // ����⨥ ��ࠡ�⪨ ������
            pressed_button = get_os_button(); // ����祭�� ���� ����⮩ ������.
            switch (pressed_button) // �஢�ઠ ����� ������ �뫠 �����
            {
                case BTN_MD5:
                    print_pending_calc(hash_str_md5);
                    sprint_hash(check_sum(MD5_BLOCK_SIZE),hash_str_md5, MD5_BLOCK_SIZE);
                    redraw_window();
                break;

                case BTN_SHA1:
                    print_pending_calc(hash_str_sha1);
                    sprint_hash(check_sum(SHA1_BLOCK_SIZE),hash_str_sha1, SHA1_BLOCK_SIZE);
                    redraw_window();
                break;

                case BTN_SHA256:
                    print_pending_calc(hash_str_sha256);
                    sprint_hash(check_sum(SHA256_BLOCK_SIZE),hash_str_sha256, SHA256_BLOCK_SIZE);
                    redraw_window();
                break;

                case BTN_COPY_MD5:
                    copy_to_clipboard(hash_str_md5);
                    redraw_window();
                break;

                case BTN_COPY_SHA1:
                    copy_to_clipboard(hash_str_sha1);
                    redraw_window();
                break;

                case BTN_COPY_SHA256:
                    copy_to_clipboard(hash_str_sha256);
                    redraw_window();
                break;

                case BTN_PASTE:
                    paste_to_edit_buffer();
                    redraw_window();
                break;

                case BTN_CMP:
                if(hash_compare())
                {
                    notify_show("'The checksum matches :)' -OK");
                    hash_edit_box.text_color = GREEN; // ��⠭�������� ⥪�� ����� ����� �᫨ ����஫쭠� �㬬� ᮢ������
                }else{
                    notify_show("'The checksum does not match! :(' -W");
                    hash_edit_box.text_color = RED; // ��⠭�������� ⥪�� ����� ���� �᫨ ����஫쭠� �㬬� �� ᮢ������
                }
                redraw_window();
                break;

                case BTN_QUIT:
                    exit(0);
                break;
            }
        }
    }while(1);
    exit(0);
}
