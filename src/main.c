/*
** EPITECH PROJECT, 2020
** replace this
** File description:
** replace this
*/

#include <kiss_fft.h>
#include "my.h"

#define WIN_SIZE_X 1920
#define WIN_SIZE_Y 1015
#define BAR_AMOUNT 30
#define BAR_WIDTH 6
#define BUCKETS 4

const int freq_bin[] = {20, 60, 250, 500};

char my_poll_event(sfRenderWindow *win)
{
    sfEvent event;

    while (sfRenderWindow_pollEvent(win, &event)) {
        if (event.type == sfEvtClosed)
            return (0);

        //if (event.type == sfEvtKeyPressed && event.key.code == sfKeyD)
        //    *x += 1;

    }
    return (1);
}

int open_sound(char *filename, sfSound **sound, sfSoundBuffer **s_buff) {

    if (*sound == NULL || (*s_buff = sfSoundBuffer_createFromFile(filename)) == NULL)
        return (1);
    sfSound_setBuffer(*sound, *s_buff);

    return (0);
}

void update_canvas(sfTexture *text, unsigned int BUFF[WIN_SIZE_X * WIN_SIZE_Y], sfInt16 *samples, int sample_count, int sample_rate, float time) {
    static int prev = 0;
    static const int N = 8000;

    if (time * sample_rate - prev < N / 2)
        return;
    
    memset(BUFF, 0, WIN_SIZE_X * WIN_SIZE_Y * 4);
    int bar_h[1000];

    int frame = (int) (time * sample_rate);
    
    if (N == 0 || frame > sample_count)
        return;
   
    kiss_fft_cpx *cx_in = malloc(sizeof(kiss_fft_cpx) * N);
    
    for (int i = 0; i < N; i++) {
        double multiplier = 0.5 * (1 - cos(2 * M_PI * i / (N - 1)));
        cx_in[i].r = multiplier * samples[i + frame];
        cx_in[i].i = 0;
    }
    
     
    kiss_fft_cfg cfg = kiss_fft_alloc( N, 0, 0, 0 );
    
    kiss_fft_cpx *cx_out = malloc(sizeof(kiss_fft_cpx) * N);
    memset(cx_out, 0, sizeof(kiss_fft_cpx) * N);
    
    
    kiss_fft( cfg , cx_in , cx_out );
    double spectrum[1000];

    int c = 0;
    for (int i = 0; i < N; i++) {
        float freq = i * sample_rate / (N * 2);
        for (int j = 0; j < BUCKETS - 1; j++) {
            if ((freq > freq_bin[j]) && (freq <= freq_bin[j + 1])) {
                spectrum[c] = sqrt(cx_out[i].r * cx_out[i].r + cx_out[i].i * cx_out[i].i);
                c++;
            }
        }
    }

    for (int i = 0; i < c; i++) {
        bar_h[i] = sqrt(spectrum[i] * 0.0002) * 3;
        if (bar_h[i] >= WIN_SIZE_Y * 0.8 - 10)
            bar_h[i] = WIN_SIZE_Y * 0.8 - 10;
    }
    for (int i = 0; i < c; i++)
        for (int j = 0; j < bar_h[i]; j++)
            for (int k = 0; k < BAR_WIDTH; k++)
                BUFF[(int)(WIN_SIZE_Y * 0.9 - j) * WIN_SIZE_X + WIN_SIZE_X / 2 + (i - c / 2) * (BAR_WIDTH + 1) + k] = -1;
    

    sfTexture_updateFromPixels(text, (sfUint8 *)BUFF,
                                    WIN_SIZE_X, WIN_SIZE_Y, 0, 0);
    kiss_fft_free(cfg);
    free(cx_in);
    free(cx_out);
}

int main(int ac, char **av)
{
    sfRenderWindow *win = sfRenderWindow_create((sfVideoMode){WIN_SIZE_X, WIN_SIZE_Y, 32},
        "Audio Visualizer", sfClose | sfResize, 0);
    sfText *txt = sfText_create();
    sfSound *sound = sfSound_create();
    sfSprite *sp = sfSprite_create();
    sfTexture *text = sfTexture_create(WIN_SIZE_X, WIN_SIZE_Y);
    sfSoundBuffer *s_buff = NULL;
    char buff[100];

    if (win == NULL || txt == NULL || sp == NULL || text == NULL || ac != 2 || open_sound(av[1], &sound, &s_buff))
        return (84);

    unsigned int BUFF[WIN_SIZE_X * WIN_SIZE_Y];

    int sample_count = sfSoundBuffer_getSampleCount(s_buff);
    int sample_rate = sfSoundBuffer_getSampleRate(s_buff);
    sfInt16 *samples = sfSoundBuffer_getSamples(s_buff);

    sfText_setFont(txt, sfFont_createFromFile("DS-DIGI.TTF"));
    sfRenderWindow_setFramerateLimit(win, 60);
    sfText_setString(txt, av[1]);
    sfSprite_setTexture(sp, text, sfTrue);
    sfSound_play(sound);

    float time = 0;

    while (sfRenderWindow_isOpen(win) && my_poll_event(win)) {
        sfRenderWindow_clear(win, sfBlack);
        time = sfTime_asSeconds(sfSound_getPlayingOffset(sound));
        int t = time;
        sprintf(buff, "  PLAYING %s\n  %02d:%02d:%02d\n", av[1], t / 60, t % 60, (int) ((time - t) * 100));
        sfText_setString(txt, buff);
        sfRenderWindow_drawText(win, txt, NULL);
        update_canvas(text, BUFF, samples, sample_count, sample_rate, time * 2);
        
        sfRenderWindow_drawSprite(win, sp, NULL);
        sfRenderWindow_drawText(win, txt, NULL);
        sfRenderWindow_display(win);
        memset(BUFF, 0, WIN_SIZE_X * WIN_SIZE_Y * 4);
    }
    sfSound_stop(sound);
    sfSoundBuffer_destroy(s_buff);
    sfSound_destroy(sound);
    sfFont_destroy(sfText_getFont(txt));
    sfText_destroy(txt);
    sfRenderWindow_destroy(win);
    return (0);
}
