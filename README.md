# Firmware pedagogique pour NUCLEO-F401RE

Ce depot est un exemple de developpement embarque de bout en bout : logique
applicative testee sur PC, pilotes HAL pour une vraie carte STM32, cross-build
ARM, script de demarrage, carte memoire et integration continue.

> Materiel vise : **NUCLEO-F401RE** avec STM32F401RE. La LED utilisateur LD2
> est sur PA5 et le bouton utilisateur B1 sur PC13.

## Ce que fait le firmware

- La LED clignote toutes les 500 ms.
- Un appui stable sur B1 active ou met en pause le clignotement.
- Le bouton est filtre par un anti-rebond logiciel de 25 ms.
- La logique du clignotement et du bouton est testee sur l'ordinateur, sans
  carte ni HAL.
- La CI compile aussi un vrai ELF pour Cortex-M4 et conserve les fichiers ELF
  et MAP comme artefacts GitHub Actions.

## Organisation du depot

| Chemin | Role |
| --- | --- |
| `src/main.c` | Point d'entree : initialise le HAL et la carte, puis execute la boucle principale. |
| `src/app.c`, `include/app.h` | Logique applicative portable : temporisation, bouton et etat de la LED. |
| `src/board_stm32f4.c`, `include/board.h` | Adaptation materielle : horloge, GPIO PA5 et bouton PC13 de la Nucleo. |
| `config/stm32f4xx_hal_conf.h` | Selection des modules STM32 HAL et configuration du HAL. |
| `linker/STM32F401RETx_FLASH.ld` | Place le code en Flash (512 Kio) et les donnees en RAM (96 Kio). |
| `cmake/arm-none-eabi-gcc.cmake` | Configure le compilateur croise ARM. |
| `CMakeLists.txt`, `Makefile` | Decrivent les cibles firmware, test et flash. |
| `test/test_app.c` | Tests de logique sur PC, y compris anti-rebond et debordement du compteur milliseconde. |
| `.github/workflows/ci.yml` | Construit le firmware et lance les tests sur chaque push et pull request. |

STM32CubeF4 est une dependance externe, pas une copie de bibliotheques ST dans
ce depot. La CI utilise la version `v1.28.3` avec ses sous-modules requis. Les
sources ST restent soumises a leur licence, distincte de la licence MIT de ce
projet.

## Prerequis

Pour compiler localement, il faut :

- CMake 3.20 ou plus recent ;
- GNU Make ;
- `arm-none-eabi-gcc` et Newlib pour la compilation embarquee ;
- Git pour telecharger STM32CubeF4 ;
- pour flasher la carte, STM32CubeProgrammer et une NUCLEO-F401RE connectee en USB.

STM32CubeIDE fournit un environnement integre avec le compilateur ARM et les
outils ST. Sur Windows, on peut aussi lancer ces commandes depuis le terminal
integre de STM32CubeIDE. La cible `make test` seule ne demande ni la carte, ni
la toolchain ARM, ni les sources STM32CubeF4.

## Compiler et tester

Depuis la racine du depot, telecharger la dependance ST :

```sh
git clone --depth 1 --branch v1.28.3 --recurse-submodules \
  https://github.com/STMicroelectronics/STM32CubeF4.git \
  third_party/STM32CubeF4
```

Dans PowerShell, definir le chemin vers cette dependance, puis compiler :

```powershell
$env:STM32CUBE_F4_DIR = (Resolve-Path .\third_party\STM32CubeF4).Path
make test
make firmware
```

`make test` utilise le compilateur natif. Les tests s'executent sur l'ordinateur.
`make firmware` utilise le compilateur ARM et produit :

- `build/firmware/firmware.elf` : firmware avec symboles de debogage ;
- `build/firmware/firmware.map` : rapport d'utilisation et placement memoire ;
- `build/firmware/firmware.bin` et `firmware.hex` : images de programmation.

Pour compiler uniquement sur GitHub, pousser le code : le workflow recupere
STM32CubeF4 et publie ces deux fichiers dans l'onglet **Actions**, sous
l'artefact `nucleo-f401re-firmware`.

## Flasher et observer

Installer STM32CubeProgrammer, connecter la Nucleo en USB, puis lancer depuis la
racine du depot :

```powershell
make flash
```

Cela utilise le ST-LINK integre a la carte pour programmer l'ELF via SWD et
redemarrer le microcontroleur. La LED LD2 doit clignoter. Appuyer sur B1 pour
mettre le clignotement en pause ou le reprendre.

Le code choisit l'horloge interne HSI (16 MHz) pour ne pas dependre d'un quartz
externe. Les numeros de broche, la polarite du bouton, la frequence d'horloge et
la taille memoire sont specifiques a cette carte : ils doivent etre revus si la
cible change.

## Comment le programme demarre

1. Apres reset, la table des vecteurs et `Reset_Handler` du paquet STM32Cube
   initialisent le processeur.
2. Le code startup copie les donnees initiales en RAM, met a zero la memoire
   `.bss`, puis appelle `main`.
3. `HAL_Init()` initialise le HAL et le compteur de millisecondes SysTick.
4. `board_init()` configure HSI et les broches de la carte.
5. La boucle de `main()` lit le temps et le bouton, demande a `app_update()`
   l'etat logique de la LED, puis le transmet au GPIO par le HAL.

L'interruption `SysTick_Handler` appelle `HAL_IncTick()`, qui fait avancer le
compteur de millisecondes. La boucle utilise `__WFI()` pour mettre le coeur en
attente jusqu'a une interruption, puis relire l'etat du bouton. Elle n'utilise
pas `HAL_Delay()` : les delais sont compares par soustraction non signee, ce qui
fonctionne aussi quand le compteur de millisecondes deborde.

## Concepts embarques illustres

- **Application et BSP/HAL** : `app.c` ne connait pas les registres STM32 ;
  `board_stm32f4.c` traduit ses commandes en acces GPIO.
- **Cross-compilation** : le PC execute `gcc` pour les tests, mais
  `arm-none-eabi-gcc` genere du code Cortex-M4 pour la carte.
- **Startup** : le fichier assembleur configure le point d'entree apres reset.
- **Linker script** : il associe code et constantes a la Flash, variables a la
  RAM, et reserve de la place pour pile et tas.
- **GPIO** : PA5 commande la LED ; PC13 est lu comme entree bouton.
- **Temps non bloquant** : SysTick fournit une base de temps; pas de boucle
  d'attente qui empeche le traitement du bouton.
- **Anti-rebond** : le bouton ne change d'etat logique que si son niveau est
  reste stable pendant 25 ms.
- **Tests unitaires** : l'application est compilee avec le compilateur du PC ;
  les tests couvrent fonctionnement normal, rebond et debordement du compteur.
- **CI** : la meme suite automatisee verifie les tests et le cross-build a chaque
  changement.

## Commandes utiles

```sh
make test      # compiler et executer les tests sur l'hote
make firmware  # compiler le firmware STM32
make flash     # programmer la carte avec STM32CubeProgrammer
make clean     # supprimer les fichiers generes
```

Un firmware de production ajouterait selon le besoin des tests de pilotes avec
des mocks, des diagnostics UART, une gestion d'erreur persistante, des
configurations de debug/release, et des tests sur carte. Ces sujets sont des
etapes suivantes, pas des prerequis au fonctionnement de cet exemple.
