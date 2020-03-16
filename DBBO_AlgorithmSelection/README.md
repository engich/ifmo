# DBBO_AlgorithmSelection
Algorithm Selection for discrete  Black-Box optimization

# Usage

Run `python3 EPM/Main.py`

By default all features will be used in model training, but you can specify

`--n_features N` i.e. `--n_features 29`

to use only `N` features

also you can list features you need

`--list_features N1 N2 N3` i.e. `--list_features 0 1 2`

or specify range

`--list_features N1:N2` i.e. `--list_features 29:49`

you can combine both

`--list_features N1 N2 N3:N4 N5` i.e. `--list_features 0 1 29:48 49`

Flacco features are in range `0:28` and paradiseo features are in range `29:49`