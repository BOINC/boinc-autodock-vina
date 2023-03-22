# boinc-autodock-vina

This application works with ZIP archive that contains data to be calculated. Input ZIP archive must contain JSON file that describes the input data and other files that are used in docking process and described in the input JSON file.

## JSON schema

This file supports next parameters:

- `receptor` - paths to receptor to be processed. This is a **optional** `strings` parameter. Only PDBQT format is supported. This file should not have absolute path. Can't be used together with `maps` parameter. Either `receptor` or `maps` **should be specified** for `vina` and `vinardo` scoring function. This parameter is **not allowed** for `ad4` scoring function.
- `ligand` - list of paths to ligands to be processed. This is an **optional** `list of strings` parameter. Either `ligand` or `batch` parameter should be specified.
- `flex` - path to PDBQT file with flexible side chains, if any. This file should not have absolute path. This is an **optional** `string` parameter for `vina` and `vinardo` scoring functions. This parameter is **required** for `ad4` scoring function.
- `batch` - paths to PDBQT files with batch ligands. This file should not have absolute path. This is an **optional** `list of string` parameters. Either `ligand` or `batch` parameter should be specified.
- `scoring` - scoring function (`ad4`, `vina` or `vinardo`). This is an **optional** `string` parameter. Default function is `vina`.
- `maps` - path to the folder with affinity maps **including prefix**. This is an **optional** `string` parameter. E.g. for the folder with maps `.\maps\1iep_receptor.A.map` and `.\maps\1iep_receptor.C.map` should be provided as `maps\1iep_receptor`. If this parameter is not specified, then `center_x`, `center_y`, `center_z` and `size_x`, `size_y`, `size_x` parameters should be specified to generate affinity maps. Can't be used together with `receptor` parameter. Either `receptor` or `maps` **should be specified** for `vina` and `vinardo` scoring function. This parameter is **required** for `ad4` scoring function.
- `center_x` - X coordinate of the center (Angstrom). This `double` parameter is ignored when `maps` parameter is specified.
- `center_y` - Y coordinate of the center (Angstrom). This `double` parameter is ignored when `maps` parameter is specified.
- `center_z` - Z coordinate of the center (Angstrom). This `double` parameter is ignored when `maps` parameter is specified.
- `size_x` - size in the X dimension (Angstrom). This `double` parameter is ignored when `maps` parameter is specified.
- `size_y` - size in the Y dimension (Angstrom). This `double` parameter is ignored when `maps` parameter is specified.
- `size_z` - size in the Z dimension (Angstrom). This `double` parameter is ignored when `maps` parameter is specified.
- `autobox` - sets maps dimensions based on input ligand(s). This parameter is used when `score_only` or `local_only` parameter is specified. This is an **optional** `boolean` parameter. Default value is `false`.
- `out` - path to output model file (PDBQT). This file should not have absolute path. This is an **optional** parameter.
- `dir` - path to output directory when: (1) in batch mode, (2) `ligand` parameter is specified and contains more than 1 file and `score_only` is not specified. This directory should not have absolute path. This is an **optional** `string` parameter.
- `write_maps` - output filename (directory + prefix name) for maps. Parameter `force_even_voxels` may be needed to comply with map format. This is an **optional** `string` parameter. E.g. for the folder with maps `.\maps\1iep_receptor.A.map` and `.\maps\1iep_receptor.C.map` should be provided as `maps\1iep_receptor`.
- `score_only` - search space can be omitted. This is an **optional** `boolean` parameter. Default value is `false`.
- `local_only` - do local search only. This is an **optional** `boolean` parameter. Default value is `false`.
- `no_refine` - when `receptor` is provided, do not use explicit receptor atoms (instead of precalculated grids) for: (1) local optimization and scoring after docking, (2) `local_only` jobs, and (3) `score_only` jobs. This is an **optional** `boolean` parameter. Default value is `false`.
- `force_even_voxels` - calculated grid maps will have an even number of voxels (intervals) in each dimension (odd number of grid points). This is an **optional** `boolean` parameter. Default value is `false`.
- `randomize_only` - randomize input, attempting to avoid clashes. This is an **optional** `boolean` parameter. Default value is `false`.
- `weight_glue` - macrocycle glue weight. This is an optional `double` parameter. Default value is `50.000000`.
- `seed` - explicit random seed. This is a **required** `integer` parameter.
- `exhaustiveness` - exhaustiveness of the global search (roughly proportional to time): 1+. This is an **optional** `integer` parameter. Default value is `8`.
- `max_evals` - number of evaluations in each MC run (if zero the number of MC steps is based on heuristics). This is an **optional** `integer` parameter. Default value is `0`.
- `num_modes` - maximum number of binding modes to generate. This is an **optional** `integer` parameter. Default value is `9`.
- `min_rmsd` - minimum RMSD between output poses. This is an **optional** `double` parameter. Default value is `1.0`.
- `energy_range` - maximum energy difference between the best binding mode and the worst one displayed (kcal/mol). This is an **optional** `double` parameter. Default value is `3.0`.
- `spacing` - grid spacing (Angstrom). This is an **optional** `double` parameter. Default value is `0.375`.

`vina` scoring function specific parameters.

- `weight_gauss1` - gauss_1 weight. This is an **optional** `double` parameter. Default value is `-0.035579`.
- `weight_gauss2` - gauss_2 weight. This is an **optional** `double` parameter. Default value is `-0.005156`.
- `weight_repulsion` - repulsion weight. This is an **optional** `double` parameter. Default value is `0.840245`.
- `weight_hydrophobic` - hydrophobic weight. This is an **optional** `double` parameter. Default value is `-0.035069`.
- `weight_hydrogen` - Hydrogen bond weight. This is an **optional** `double` parameter. Default value is `-0.587439`.
- `weight_rot` - N_rot weight. This is an **optional** `double` parameter. Default value is `0.05846`.

`vinardo` scoring function specific parameters.

- `weight_vinardo_gauss1` - gauss_1 weight. This is an **optional** `double` parameter. Default value is `-0.045`.
- `weight_vinardo_repulsion` - repulsion weight. This is an **optional** `double` parameter. Default value is `0.8`.
- `weight_vinardo_hydrophobic` - hydrophobic weight. This is an **optional** `double` parameter. Default value is `-0.035`.
- `weight_vinardo_hydrogen` - Hydrogen bond weight. This is an **optional** `double` parameter. Default value is `0.600`.
- `weight_vinardo_rot` - N_rot weight. This is an **optional** `double` parameter. Default value is `0.05846`.

`ad4` scoring function specific parameters.

- `weight_ad4_vdw` - ad4_vdw weight. This is an **optional** `double` parameter. Default value is `0.1662`.
- `weight_ad4_hb` - ad4_hb weight. This is an **optional** `double` parameter. Default value is `0.1209`.
- `weight_ad4_elec` - ad4_elec weight. This is an **optional** `double` parameter. Default value is `0.1406`.
- `weight_ad4_dsolv` - ad4_dsolv weight. This is an **optional** `double` parameter. Default value is `0.1322`.
- `weight_ad4_rot` - ad4_rot weight. This is an **optional** `double` parameter. Default value is `0.2983`.

More information about specified parameters could be found on the official documentation webpage of Autodock-Vina: https://autodock-vina.readthedocs.io/en/latest/index.html

### Minimal JSON file example

```json
{
    "receptor": "1iep_receptor.pdbqt",
    "ligand": "1iep_ligand.pdbqt",
    "center_x": 15.190,
    "center_y": 53.903,
    "center_z": 16.917,
    "size_x": 20.0,
    "size_y": 20.0,
    "size_z": 20.0,
    "seed": 123456,
    "out": "1iep_ligand_vina_out.pdbqt"
}
```
