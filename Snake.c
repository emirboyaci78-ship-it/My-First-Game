#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>


#define EKRAN_GENISLIK 1920
#define EKRAN_YUKSEKLIK 1080
#define IZGARA_BOYUTU 30

typedef struct {
    int x;
    int y;
} Pozisyon;

typedef struct {
    Pozisyon govde[1200];
    int uzunluk;
    Pozisyon hiz;
    bool canli;
} Yilan;

typedef struct {
    Pozisyon poz;
    bool aktif;
} Yem;

typedef struct {
    int puan;
} SkorKaydi;

// Fonksiyonlar
void OyunuGuncelle(Yilan *y, Yem *g, int *skor);
void OyunuCiz(Yilan y, Yem g, int skor, int yuksekSkor, int durum);
void SkoruKaydet(int yeniSkor);
int YuksekSkorOku();

int main(void) {
    InitWindow(EKRAN_GENISLIK, EKRAN_YUKSEKLIK, "C Projesi: Yilan Oyunu (Full HD)");
    SetTargetFPS(15);

    Yilan yilan = {0};
    yilan.uzunluk = 1;
    yilan.govde[0] = (Pozisyon){ (EKRAN_GENISLIK/IZGARA_BOYUTU)/2, (EKRAN_YUKSEKLIK/IZGARA_BOYUTU)/2 };
    yilan.hiz = (Pozisyon){1, 0};
    yilan.canli = true;

    Yem yem = {(Pozisyon){20, 15}, true};
    int mevcutSkor = 0;
    int yuksekSkor = YuksekSkorOku();
    int oyunDurumu = 0; // 0: Baslik, 1: Oynaniyor, 2: Bitti

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_RIGHT) && yilan.hiz.x == 0) yilan.hiz = (Pozisyon){1, 0};
        if (IsKeyPressed(KEY_LEFT) && yilan.hiz.x == 0) yilan.hiz = (Pozisyon){-1, 0};
        if (IsKeyPressed(KEY_UP) && yilan.hiz.y == 0) yilan.hiz = (Pozisyon){0, -1};
        if (IsKeyPressed(KEY_DOWN) && yilan.hiz.y == 0) yilan.hiz = (Pozisyon){0, 1};

        if (oyunDurumu == 0 && IsKeyPressed(KEY_ENTER)) oyunDurumu = 1;

        if (oyunDurumu == 2 && IsKeyPressed(KEY_R)) {
            yilan.uzunluk = 1;
            yilan.govde[0] = (Pozisyon){ (EKRAN_GENISLIK/IZGARA_BOYUTU)/2, (EKRAN_YUKSEKLIK/IZGARA_BOYUTU)/2 };
            yilan.canli = true;
            yilan.hiz = (Pozisyon){1, 0};
            mevcutSkor = 0;
            oyunDurumu = 1;
        }

        if (oyunDurumu == 1) {
            OyunuGuncelle(&yilan, &yem, &mevcutSkor);
            if (!yilan.canli) {
                oyunDurumu = 2;
                SkoruKaydet(mevcutSkor);
                yuksekSkor = YuksekSkorOku();
            }
        }

        OyunuCiz(yilan, yem, mevcutSkor, yuksekSkor, oyunDurumu);
    }

    CloseWindow();
    return 0;
}

void OyunuGuncelle(Yilan *y, Yem *g, int *skor) {
    for (int i = y->uzunluk - 1; i > 0; i--) y->govde[i] = y->govde[i - 1];
    y->govde[0].x += y->hiz.x;
    y->govde[0].y += y->hiz.y;

    if (y->govde[0].x < 0 || y->govde[0].x >= EKRAN_GENISLIK / IZGARA_BOYUTU ||
        y->govde[0].y < 0 || y->govde[0].y >= EKRAN_YUKSEKLIK / IZGARA_BOYUTU) {
        y->canli = false;
    }

    for (int i = 1; i < y->uzunluk; i++) {
        if (y->govde[0].x == y->govde[i].x && y->govde[0].y == y->govde[i].y) y->canli = false;
    }

    if (y->govde[0].x == g->poz.x && y->govde[0].y == g->poz.y) {
        y->uzunluk++;
        *skor += 10;
        g->poz.x = GetRandomValue(0, (EKRAN_GENISLIK / IZGARA_BOYUTU) - 1);
        g->poz.y = GetRandomValue(0, (EKRAN_YUKSEKLIK / IZGARA_BOYUTU) - 1);
    }
}

void SkoruKaydet(int yeniSkor) {
    SkorKaydi skorlar[5] = {0};
    int sayac = 0;
    FILE *dosya = fopen("yuksek_skorlar.dat", "rb");
    if (dosya != NULL) {
        while (fread(&skorlar[sayac], sizeof(SkorKaydi), 1, dosya) && sayac < 4) sayac++;
        fclose(dosya);
    }
    skorlar[sayac].puan = yeniSkor;
    sayac++;
    for (int i = 0; i < sayac - 1; i++) {
        for (int j = 0; j < sayac - i - 1; j++) {
            if (skorlar[j].puan < skorlar[j + 1].puan) {
                SkorKaydi gecici = skorlar[j];
                skorlar[j] = skorlar[j + 1];
                skorlar[j + 1] = gecici;
            }
        }
    }
    dosya = fopen("yuksek_skorlar.dat", "wb");
    if (dosya != NULL) {
        for (int i = 0; i < (sayac < 5 ? sayac : 5); i++) fwrite(&skorlar[i], sizeof(SkorKaydi), 1, dosya);
        fclose(dosya);
    }
}

int YuksekSkorOku() {
    SkorKaydi enIyi = {0};
    FILE *dosya = fopen("yuksek_skorlar.dat", "rb");
    if (dosya == NULL) return 0;
    fread(&enIyi, sizeof(SkorKaydi), 1, dosya);
    fclose(dosya);
    return enIyi.puan;
}

void OyunuCiz(Yilan y, Yem g, int skor, int yuksekSkor, int durum) {
    BeginDrawing();
    ClearBackground(BLACK);

    if (durum == 0) {
        DrawText("YILAN OYUNU", EKRAN_GENISLIK/2 - 200, EKRAN_YUKSEKLIK/2 - 100, 60, GREEN);
        DrawText("BASLAMAK ICIN ENTER'A BASIN", EKRAN_GENISLIK/2 - 250, EKRAN_YUKSEKLIK/2 + 20, 30, WHITE);
    }
    else if (durum == 1) {
        DrawRectangle(g.poz.x * IZGARA_BOYUTU, g.poz.y * IZGARA_BOYUTU, IZGARA_BOYUTU, IZGARA_BOYUTU, RED);
        for (int i = 0; i < y.uzunluk; i++) {
            DrawRectangle(y.govde[i].x * IZGARA_BOYUTU, y.govde[i].y * IZGARA_BOYUTU, IZGARA_BOYUTU - 1, IZGARA_BOYUTU - 1, LIME);
        }
        DrawText(TextFormat("SKOR: %d", skor), 30, 30, 30, RAYWHITE);
        DrawText(TextFormat("EN IYI: %d", yuksekSkor), 30, 70, 30, YELLOW);
    }
    else if (durum == 2) {
        DrawText("OYUN BITTI!", EKRAN_GENISLIK/2 - 180, EKRAN_YUKSEKLIK/2 - 100, 60, RED);
        DrawText(TextFormat("SKORUNUZ: %d", skor), EKRAN_GENISLIK/2 - 110, EKRAN_YUKSEKLIK/2, 30, WHITE);
        DrawText("YENIDEN BASLAMAK ICIN 'R' TUSUNA BASIN", EKRAN_GENISLIK/2 - 320, EKRAN_YUKSEKLIK/2 + 80, 30, LIGHTGRAY);
    }

    EndDrawing();
}
