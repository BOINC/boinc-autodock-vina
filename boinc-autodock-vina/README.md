# boinc-autodock-vina

This application supports only one input parameter: JSON file that describes the input data.

## JSON schema

This file supports 5 groups of parameters:

- `input` - input PDBQT data. This is an **optional** parameter.
- `search_area` - search area. This is a **required** parameter.
- `output` - output data. This is an **optional** parameter.
- `advanced` - advanced parameters. This is a **required** parameter.
- `misc` - miscellaneous parameters. This is an **optional** parameter.

### **input** parameters group

This group supports next parameters:

- `receptor` - PDBQT file with rigid part of the receptor. This file should not have absolute path. This is an **optional** `string` parameter. Can't be used together with `search_area.maps` parameter. Either `input.receptor` or `search_area.maps` **should be specified** for `vina` and `vinardo` scoring function. This parameter is **not allowed** for `ad4` scoring function.
- `flex` - PDBQT file with flexible side chains, if any. This file should not have absolute path. This is an **optional** `string` parameter for `vina` and `vinardo` scoring functions. This parameter is **required** for `ad4` scoring function.
- `ligands` - PDBQT files with ligands. This file should not have absolute path. This is an **optional** `list of string` parameters. Either `input.ligands` or `input.batch` parameter should be specified.
- `batch` - PDBQT files with batch ligands. This file should not have absolute path. This is an **optional** `list of string` parameters. Either `input.ligands` or `input.batch` parameter should be specified.
- `scoring` - scoring function (`ad4`, `vina` or `vinardo`). This is an **optional** `string` parameter. Default function is `vina`.

### **search_area** parameters group

This group supports next parameters:

- `maps` - path to the folder with affinity maps **including prefix**. This is an **optional** `string` parameter. E.g. for the folder with maps `.\maps\1iep_receptor.A.map` and `.\maps\1iep_receptor.C.map` should be provided as `maps\1iep_receptor`. If this parameter is not specified, then `search_area.center_x`, `search_area.center_y`, `search_area.center_z` and `search_area.size_x`, `search_area.size_y`, `search_area.size_x` parameters should be specified to generate affinity maps. Can't be used together with `input.receptor` parameter. Either `input.receptor` or `search_area.maps` **should be specified** for `vina` and `vinardo` scoring function. This parameter is **required** for `ad4` scoring function.
- `center_x` - X coordinate of the center (Angstrom). This `double` parameter is ignored when `search_area.maps` parameter is specified.
- `center_y` - Y coordinate of the center (Angstrom). This `double` parameter is ignored when `search_area.maps` parameter is specified.
- `center_z` - Z coordinate of the center (Angstrom). This `double` parameter is ignored when `search_area.maps` parameter is specified.
- `size_x` - size in the X dimension (Angstrom). This `double` parameter is ignored when `search_area.maps` parameter is specified.
- `size_y` - size in the Y dimension (Angstrom). This `double` parameter is ignored when `search_area.maps` parameter is specified.
- `size_z` - size in the Z dimension (Angstrom). This `double` parameter is ignored when `search_area.maps` parameter is specified.
- `autobox` - sets maps dimensions based on input ligand(s). This parameter is used when `advanced.score_only` or `advanced.local_only` parameter is specified. This is an **optional** `boolean` parameter. Default value is `false`.

### **output** parameters group

This group supports next parameters:

- `out` - output model files (PDBQT), the default is chosen based on the ligand file name. This file should not have absolute path. This is an **required** `string` parameter when: (1) `input.batch` parameter is specified, (2) `input.ligands` contains more than 1 file and `advanced.score_only` is not specified, otherwise it is **optional**.
- `dir` - output directory for batch mode.  This directory should not have absolute path. This is an **optional** `string` parameter.
- `write_maps` - output filename (directory + prefix name) for maps. Parameter `advanced.force_even_voxels` may be needed to comply with map format. This is an **optional** `string` parameter. E.g. for the folder with maps `.\maps\1iep_receptor.A.map` and `.\maps\1iep_receptor.C.map` should be provided as `maps\1iep_receptor`.

### **advanced** parameters group

This group supports next parameters:

- `score_only` - score only - search space can be omitted. This is an **optional** `boolean` parameter. Default value is `false`.
- `local_only` - do local search only. This is an **optional** `boolean` parameter. Default value is `false`.
- `no_refine` - when `input.receptor` is provided, do not use explicit receptor atoms (instead of precalculated grids) for: (1) local optimization and scoring after docking, (2) `advanced.local_only` jobs, and (3) `advanced.score_only` jobs. This is an **optional** `boolean` parameter. Default value is `false`.
- `force_even_voxels` - calculated grid maps will have an even number of voxels (intervals) in each dimension (odd number of grid points). This is an **optional** `boolean` parameter. Default value is `false`.
- `randomize_only` - randomize input, attempting to avoid clashes. This is an **optional** `boolean` parameter. Default value is `false`.
- `weight_glue` - macrocycle glue weight. This is an optional `double` parameter. Default value is `50.000000`.

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

### **misc** parameters group

This group supports next parameters:

- `seed` - explicit random seed. This is an **optional** `integer` parameter. Default value is `0`.
- `exhaustiveness` - exhaustiveness of the global search (roughly proportional to time): 1+. This is an **optional** `integer` parameter. Default value is `8`.
- `max_evals` - number of evaluations in each MC run (if zero the number of MC steps is based on heuristics). This is an **optional** `integer` parameter. Default value is `0`.
- `num_modes` - maximum number of binding modes to generate. This is an **optional** `integer` parameter. Default value is `9`.
- `min_rmsd` - minimum RMSD between output poses. This is an **optional** `double` parameter. Default value is `1.0`.
- `energy_range` - maximum energy difference between the best binding mode and the worst one displayed (kcal/mol). This is an **optional** `double` parameter. Default value is `3.0`.
- `spacing` - grid spacing (Angstrom). This is an **optional** `double` parameter. Default value is `0.375`.

More information about specified parameters could be found on the official documentation webpage of Autodock-Vina: https://autodock-vina.readthedocs.io/en/latest/index.html
