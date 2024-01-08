# sfml-tiny-projects
A collection of tiny projects made with [SFML](https://en.wikipedia.org/wiki/Simple_and_Fast_Multimedia_Library) in [C++](https://en.wikipedia.org/wiki/C%2B%2B) as part of the course [BMA1020 - Mathematics for Programming](www.ntnu.edu/studies/courses/BMA1020), 2020.<br>

## Programs
### "Agario"
![Video depicting game which looks like Agario](https://github.com/Thefantasticbagle/sfml-tiny-projects/blob/9b720ef88afbd94756f8e2ecbafc889c5cce42ff/images/agario_gameplay.gif)

A short game made to resemble [Agar.io](https://en.wikipedia.org/wiki/Agar.io). The player uses their mass to propel themselves and consume smaller blobs, until they are the only one remaining or are eliminated.

```
$ make program NAME=agario
$ ./agario
```

### Interactive spline
![Video depicting interactive spline program](https://github.com/Thefantasticbagle/sfml-tiny-projects/blob/9b720ef88afbd94756f8e2ecbafc889c5cce42ff/images/spline_editor_example.gif)

Add, move, or remove the weights of a spline. Touching the spline with the mouse sprouts particles in the direction of the "river" where the mouse currently is.

```
$ make program NAME=spline_editor
$ ./spline_editor
```

### Noise (flat)
![Video depicting flat noise program](https://github.com/Thefantasticbagle/sfml-tiny-projects/blob/9b720ef88afbd94756f8e2ecbafc889c5cce42ff/images/noise_flat_example.gif)

Add layers of noise and watch a ball roll accross it.

```
$ make program NAME=noise_flat
$ ./noise_flat
```

### Noise (round)
![Image depicting round noise program](https://github.com/Thefantasticbagle/sfml-tiny-projects/blob/9b720ef88afbd94756f8e2ecbafc889c5cce42ff/images/noise_round_example.png)

Visualize noise on a circle. Angle determines frequency, and distance to the centre amplitude.

```
$ make program NAME=noise_round
$ ./noise_round
```

### Windy balls
![Video depicting windy balls program](https://github.com/Thefantasticbagle/sfml-tiny-projects/blob/9b720ef88afbd94756f8e2ecbafc889c5cce42ff/images/windy_balls_example.gif)

Balls which bounce and fall with the wind, smaller ones are affected more.

```
$ make program NAME=windy_balls
$ ./windy_balls
```

## Setup
### Downloading the repository
```sh
$ git clone https://github.com/Thefantasticbagle/sfml-tiny-projects.git
$ cd sfml-tiny-projects
```

### Installing dependencies
To run any of the projects, SFML is required.
```sh
$ sudo apt-get install libsfml-dev
```

### Run
```sh
$ make program NAME=insert_name_here
$ ./insert_name_here
```
Builds and runs any of the given projects in the `src` folder. To clean up all buildsfiles, use `make clean`.
