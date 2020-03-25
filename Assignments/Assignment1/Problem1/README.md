### Parallel Bubble Sort in MPI

- To run the program, run makefile  using
    ```
        make np=<num_of_processors_required>
        e.g. make np=10
    ```

- Input is stored in <b> input.txt </b> with format as
    ```
        num_of_elems,elem_1,elem_2,.....,elem_n
    ```

- If you get an error like
    ```
        There are not enough slots available in the system to satisfy the xxx slots that were requested by the application: Either request fewer slots for your application, or make more slots available for use.
    ```

    - Just type echo "localhost slots=< a number bigger than xxx >" > hostfile
    - Rerun make, it should work now

### Algorithm applied 

- The program divides number of elements equally to processors
    - i.e. with n elements and p processors, each processor gets n/p elements

- Each processor sequentially sorts its n/p elements using merge sort

- Then the processors do odd-even transposition sort
    ```
        for p phases do
            
            if phase is odd begin
                odd rank processors exchange data with thier next processor and keep lower elements
            end

            else begin
                even rank processors exchange with their next processors and keep lower
            end        

        endfor
    ```

### Theoretical speedup possible

- Time complexity of serial execution 
    
    - n = to take input
    - 3n(n-1)/2 = for sorting
    - which is O(n^2)

- Time complexity of parallel execution

    - n = to take input
    - O(n/p log(n/p)) - for locally sorting using merge sort
    - p*( 2*n/p{for send/rcv operation} + 2n/p {picking lower/higher elements} + communication costs ) = O(n) if we ignore communication costs
    - which is O(n) if p ~ n

- Therefore, theoretical speedup = O(n^2) / O(n) = n

### Actual speedup

- For Input size = 40000

    - np = 2, seq time = 2.546008, parallel time = 0.029484, speedup = 86.3529
    - np = 5, seq time = 2.929127, parallel time = 0.048237, speedup = 60.7239
    - np = 10, seq time = 3.222945, parallel time = 1.496682, speedup = 2.143394

- Mismatch in experimental and actual speedup may be because of increasing communication costs while increasing the number of processors. (Since, we have ignored the communication costs in analysis)

### Dependencies
- OpenMPI
    - To install run command ``` sudo apt install openmpi-bin ```

### Example Run
<img src="example_run.gif" alt="example run gif">