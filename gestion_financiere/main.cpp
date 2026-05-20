#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* ========== VARIABLES ========== */
float solde = 0.0;

#define ID_BTN_DEPOT       1
#define ID_BTN_RETRAIT     2
#define ID_BTN_QUITTER     3
#define ID_EDIT_MONTANT    4
#define ID_STATIC_SOLDE    5
#define ID_STATIC_MSG      6
#define ID_LIST_HISTORIQUE 7

HWND hSolde, hMontant, hMsg, hList;
HFONT hFontTitre, hFontNormal, hFontSolde;

/* ========== COULEURS ========== */
#define CLR_BG        RGB(30, 30, 50)
#define CLR_PANEL     RGB(45, 45, 70)
#define CLR_VERT      RGB(39, 174, 96)
#define CLR_ROUGE     RGB(192, 57, 43)
#define CLR_GRIS      RGB(100, 100, 120)
#define CLR_TEXTE     RGB(255, 255, 255)
#define CLR_SOLDE     RGB(241, 196, 15)
#define CLR_MSG_OK    RGB(39, 174, 96)
#define CLR_MSG_ERR   RGB(231, 76, 60)

COLORREF couleur_msg = RGB(255,255,255);

/* ========== SAUVEGARDE ========== */
void sauvegarder_solde() {
    FILE *f = fopen("solde.txt", "w");
    if (f) { fprintf(f, "%.2f", solde); fclose(f); }
}

void charger_solde() {
    FILE *f = fopen("solde.txt", "r");
    if (f) { fscanf(f, "%f", &solde); fclose(f); }
}

void sauvegarder_transaction(float montant, char type[]) {
    FILE *f = fopen("historique.txt", "a");
    if (!f) return;
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char date[30];
    strftime(date, 30, "%d/%m/%Y %H:%M:%S", tm_info);
    fprintf(f, "[%s] %s : %.2f FCFA\n", date, type, montant);
    fclose(f);
}

/* ========== AFFICHAGE ========== */
void maj_solde_affichage() {
    char buf[100];
    sprintf(buf, "Solde actuel : %.2f FCFA", solde);
    SetWindowText(hSolde, buf);
}

void afficher_message(const char *msg, int ok) {
    couleur_msg = ok ? CLR_MSG_OK : CLR_MSG_ERR;
    SetWindowText(hMsg, msg);
    InvalidateRect(hMsg, NULL, TRUE);
}

void charger_historique_list() {
    SendMessage(hList, LB_RESETCONTENT, 0, 0);
    FILE *f = fopen("historique.txt", "r");
    if (!f) {
        SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)"  Aucune transaction enregistree.");
        return;
    }
    char ligne[200];
    while (fgets(ligne, sizeof(ligne), f)) {
        int len = strlen(ligne);
        if (len > 0 && ligne[len-1] == '\n') ligne[len-1] = '\0';
        SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)ligne);
    }
    fclose(f);
}

/* ========== OPERATIONS ========== */
void faire_depot() {
    char buf[50];
    GetWindowText(hMontant, buf, 50);
    float montant = atof(buf);
    if (montant <= 0) { afficher_message("Montant invalide !", 0); return; }
    solde += montant;
    sauvegarder_transaction(montant, "Depot");
    sauvegarder_solde();
    maj_solde_affichage();
    char msg[100];
    sprintf(msg, "Depot de %.2f FCFA effectue avec succes !", montant);
    afficher_message(msg, 1);
    SetWindowText(hMontant, "");
    charger_historique_list();
}

void faire_retrait() {
    char buf[50];
    GetWindowText(hMontant, buf, 50);
    float montant = atof(buf);
    if (montant <= 0) { afficher_message("Montant invalide !", 0); return; }
    if (montant > solde) { afficher_message("Solde insuffisant !", 0); return; }
    solde -= montant;
    sauvegarder_transaction(montant, "Retrait");
    sauvegarder_solde();
    maj_solde_affichage();
    char msg[100];
    sprintf(msg, "Retrait de %.2f FCFA effectue avec succes !", montant);
    afficher_message(msg, 1);
    SetWindowText(hMontant, "");
    charger_historique_list();
}

/* ========== DESSIN BOUTONS COLORES ========== */
void dessiner_bouton(DRAWITEMSTRUCT *dis, COLORREF couleur, const char *texte) {
    HDC hdc = dis->hDC;
    RECT rc = dis->rcItem;

    COLORREF clr = (dis->itemState & ODS_SELECTED) ? RGB(
        (BYTE)(GetRValue(couleur) * 0.7),
        (BYTE)(GetGValue(couleur) * 0.7),
        (BYTE)(GetBValue(couleur) * 0.7)
    ) : couleur;

    HBRUSH hBrush = CreateSolidBrush(clr);
    FillRect(hdc, &rc, hBrush);
    DeleteObject(hBrush);

    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(255,255,255));
    SelectObject(hdc, hPen);
    RoundRect(hdc, rc.left, rc.top, rc.right, rc.bottom, 10, 10);
    DeleteObject(hPen);

    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, CLR_TEXTE);
    SelectObject(hdc, hFontNormal);
    DrawText(hdc, texte, -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

/* ========== PROCEDURE FENETRE ========== */
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch(msg) {

        case WM_CREATE: {
            hFontTitre  = CreateFont(22, 0, 0, 0, FW_BOLD, 0, 0, 0,
                            DEFAULT_CHARSET, 0, 0, 0, 0, "Segoe UI");
            hFontSolde  = CreateFont(18, 0, 0, 0, FW_BOLD, 0, 0, 0,
                            DEFAULT_CHARSET, 0, 0, 0, 0, "Segoe UI");
            hFontNormal = CreateFont(14, 0, 0, 0, FW_SEMIBOLD, 0, 0, 0,
                            DEFAULT_CHARSET, 0, 0, 0, 0, "Segoe UI");

            HWND hTitre = CreateWindow("STATIC", "GESTION FINANCIERE",
                WS_VISIBLE | WS_CHILD | SS_CENTER,
                0, 15, 520, 30, hwnd, NULL, NULL, NULL);
            SendMessage(hTitre, WM_SETFONT, (WPARAM)hFontTitre, TRUE);

            hSolde = CreateWindow("STATIC", "Solde actuel : 0.00 FCFA",
                WS_VISIBLE | WS_CHILD | SS_CENTER,
                50, 55, 420, 28, hwnd, (HMENU)ID_STATIC_SOLDE, NULL, NULL);
            SendMessage(hSolde, WM_SETFONT, (WPARAM)hFontSolde, TRUE);

            HWND hLabel = CreateWindow("STATIC", "Montant (FCFA) :",
                WS_VISIBLE | WS_CHILD,
                50, 103, 150, 22, hwnd, NULL, NULL, NULL);
            SendMessage(hLabel, WM_SETFONT, (WPARAM)hFontNormal, TRUE);

            hMontant = CreateWindow("EDIT", "",
                WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,
                200, 100, 170, 26, hwnd, (HMENU)ID_EDIT_MONTANT, NULL, NULL);
            SendMessage(hMontant, WM_SETFONT, (WPARAM)hFontNormal, TRUE);

            CreateWindow("BUTTON", "Deposer",
                WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
                50, 145, 130, 38, hwnd, (HMENU)ID_BTN_DEPOT, NULL, NULL);

            CreateWindow("BUTTON", "Retirer",
                WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
                195, 145, 130, 38, hwnd, (HMENU)ID_BTN_RETRAIT, NULL, NULL);

            CreateWindow("BUTTON", "Quitter",
                WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
                340, 145, 130, 38, hwnd, (HMENU)ID_BTN_QUITTER, NULL, NULL);

            hMsg = CreateWindow("STATIC", "",
                WS_VISIBLE | WS_CHILD | SS_CENTER,
                50, 195, 420, 22, hwnd, (HMENU)ID_STATIC_MSG, NULL, NULL);
            SendMessage(hMsg, WM_SETFONT, (WPARAM)hFontNormal, TRUE);

            HWND hLblHist = CreateWindow("STATIC", "Historique des transactions :",
                WS_VISIBLE | WS_CHILD,
                50, 225, 300, 20, hwnd, NULL, NULL, NULL);
            SendMessage(hLblHist, WM_SETFONT, (WPARAM)hFontNormal, TRUE);

            hList = CreateWindow("LISTBOX", NULL,
                WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL | LBS_NOTIFY,
                50, 250, 420, 180, hwnd, (HMENU)ID_LIST_HISTORIQUE, NULL, NULL);
            SendMessage(hList, WM_SETFONT, (WPARAM)hFontNormal, TRUE);

            maj_solde_affichage();
            charger_historique_list();
            break;
        }

        case WM_CTLCOLORSTATIC: {
            HDC hdc = (HDC)wParam;
            HWND hCtrl = (HWND)lParam;
            SetBkMode(hdc, TRANSPARENT);
            if (hCtrl == hSolde) {
                SetTextColor(hdc, CLR_SOLDE);
            } else if (hCtrl == hMsg) {
                SetTextColor(hdc, couleur_msg);
            } else {
                SetTextColor(hdc, CLR_TEXTE);
            }
            return (LRESULT)CreateSolidBrush(CLR_BG);
        }

        case WM_CTLCOLOREDIT: {
            HDC hdc = (HDC)wParam;
            SetBkColor(hdc, CLR_PANEL);
            SetTextColor(hdc, CLR_TEXTE);
            return (LRESULT)CreateSolidBrush(CLR_PANEL);
        }

        case WM_CTLCOLORLISTBOX: {
            HDC hdc = (HDC)wParam;
            SetBkColor(hdc, CLR_PANEL);
            SetTextColor(hdc, CLR_TEXTE);
            return (LRESULT)CreateSolidBrush(CLR_PANEL);
        }

        case WM_DRAWITEM: {
            DRAWITEMSTRUCT *dis = (DRAWITEMSTRUCT*)lParam;
            switch(dis->CtlID) {
                case ID_BTN_DEPOT:   dessiner_bouton(dis, CLR_VERT,  "Deposer"); break;
                case ID_BTN_RETRAIT: dessiner_bouton(dis, CLR_ROUGE, "Retirer"); break;
                case ID_BTN_QUITTER: dessiner_bouton(dis, CLR_GRIS,  "Quitter"); break;
            }
            return TRUE;
        }

        case WM_ERASEBKGND: {
            HDC hdc = (HDC)wParam;
            RECT rc;
            GetClientRect(hwnd, &rc);
            HBRUSH hBrush = CreateSolidBrush(CLR_BG);
            FillRect(hdc, &rc, hBrush);
            DeleteObject(hBrush);
            return 1;
        }

        case WM_COMMAND: {
            switch(LOWORD(wParam)) {
                case ID_BTN_DEPOT:   faire_depot();       break;
                case ID_BTN_RETRAIT: faire_retrait();     break;
                case ID_BTN_QUITTER: DestroyWindow(hwnd); break;
            }
            break;
        }

        case WM_DESTROY:
            DeleteObject(hFontTitre);
            DeleteObject(hFontNormal);
            DeleteObject(hFontSolde);
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

/* ========== MAIN ========== */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
    charger_solde();

    WNDCLASS wc = {0};
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = "GestionFinanciere";
    wc.hbrBackground = (HBRUSH)CreateSolidBrush(CLR_BG);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    RegisterClass(&wc);

    HWND hwnd = CreateWindow(
        "GestionFinanciere", "Gestion Financiere - Groupe",
        WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 540, 490,
        NULL, NULL, hInstance, NULL);

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while(GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}
