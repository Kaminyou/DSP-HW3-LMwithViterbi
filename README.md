# DSP-HW3-LMwithViterbi
## Usage
1. Use the docker image and attach the container. Please revise the location to mount this folder first!
```script
./set_up_docker.sh
```
2. Switch into this folder after attach the container
```
cd /root/dsp_hw3/
```
3. Activate SRILM environment
```
source setup.sh
```
4. Prepare everything required
```
./run.sh
```
5. Prepare decoding binary
```
make
```
6. Prepare mapping file
```
make map
```
7. Decoding
```
./test_mydisambig.sh
```
8. For the whole experiment
```
python3 run_all_experiment.py
```