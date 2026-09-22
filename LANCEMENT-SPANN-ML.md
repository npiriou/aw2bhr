# Lancer Spann Island avec le modèle ML

La ROM modifiée ne doit pas être ouverte directement avec le mGBA installé dans `C:\Program Files`. Le modèle a besoin du lanceur, qui démarre automatiquement la bonne version de mGBA, le pont Lua et le processus Python.

## Procédure normale

1. Fermer toutes les fenêtres mGBA déjà ouvertes.
2. Ouvrir le dossier `X:\dev\aw2bhr-spann-local` dans l'Explorateur Windows.
3. Double-cliquer sur `START-SPANN-ML.cmd`.
4. Le lanceur copie automatiquement les réglages de `C:\Users\npiri\AppData\Roaming\mGBA` vers la version portable. La manette, le clavier, l'audio et la vidéo doivent donc utiliser les mêmes paramètres que le mGBA normal.
5. Garder la fenêtre de commande ouverte. Elle héberge le lanceur pendant la partie.
6. Attendre que mGBA s'ouvre. Le lanceur attend d'abord que runE-U83 soit chargé, ce qui peut prendre quelques secondes.
7. Dans le jeu, choisir **War Room**, puis **Spann Island**.
8. Jouer normalement et terminer le tour humain. Au tour ennemi, runE-U83 envoie ses décisions à AW2; les déplacements, captures, constructions et changements de tour sont exécutés par le moteur original du jeu.
9. Pour arrêter, fermer mGBA. Le processus Python local est alors arrêté automatiquement.

## À ne pas faire

Ne pas double-cliquer sur `build-mod\aw2bhr-spann-local.gba`. L'association Windows ouvre actuellement mGBA 0.10.5, qui ne charge pas automatiquement le script Lua. La ROM repasse maintenant sur l'IA native après environ trois secondes si le pont ML est absent, mais le modèle runE-U83 ne sera pas utilisé.

## En cas de problème

Le journal principal est `X:\dev\aw2bhr-spann-local\runtime\bridge.stderr.log`. Les lignes suivantes indiquent un démarrage correct :

```text
loading model once from ...runE-U83-inference.pt
ready: mode=model runtime=...
```

Après un tour ennemi, le même fichier doit contenir des lignes `session=... request=... action=... native=...`.

Pour afficher davantage de transitions, lancer depuis PowerShell :

```powershell
cd X:\dev\aw2bhr-spann-local
.\tools\run_spann_local.ps1 -Mode model -Trace
```
