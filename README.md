# Exemple minimal embarque + GitHub Actions

Ce projet illustre une CI simple pour un petit projet embarque.

## Contenu

- `src/` : application minimale et HAL simulee pour piloter une LED.
- `test/` : test unitaire compile et execute sur l'hote.
- `Makefile` : `make all` produit un ELF ARM et `make test` execute le test local.
- `.github/workflows/ci.yml` : installe la toolchain ARM, compile le firmware et lance le test hote.

## Utilisation locale (Ubuntu)

Installer les dependances :

```sh
sudo apt-get update
sudo apt-get install -y build-essential gcc-arm-none-eabi make
```

Compiler pour ARM :

```sh
make all
```

Executer les tests sur l'hote :

```sh
make test
```

Nettoyer les fichiers generes :

```sh
make clean
```

Le test s'execute sur la machine hote avec `gcc`, ce qui permet de valider la
logique de la HAL simulee sans materiel. Le cross-build est un exemple minimal
et ne fournit pas de startup code, de script de linker specifique a une carte,
ni d'image directement flashable. Pour un firmware reel, ces elements doivent
etre adaptes a la cible.
