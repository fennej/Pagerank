#include <windows.h>
#include <stdio.h>

// La fonction qui sera exécutée par le thread
DWORD WINAPI MaFonctionThread(LPVOID lpParam) {
    printf("Salut depuis le thread ! Parametre : %d\n", *(int*)lpParam);
    return 0;
}

int main() {
    int data = 42;
    HANDLE hThread;
    DWORD dwThreadId;

    // Création du thread
    hThread = CreateThread(
        NULL,                   // Sécurité par défaut
        0,                      // Taille de pile par défaut
        MaFonctionThread,       // Fonction à exécuter
        &data,                  // Argument passé à la fonction
        0,                      // Drapeaux de création
        &dwThreadId             // Variable pour recevoir l'ID du thread
    );

    if (hThread == NULL) {
        return 1; // Erreur
    }

    // Attendre que le thread se termine (équivalent de join)
    WaitForSingleObject(hThread, INFINITE);

    // Toujours fermer le handle quand on a fini
    CloseHandle(hThread);

    return 0;
}