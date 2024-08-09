## Part `II`

> Find the k-th ancestor of each node in an up-pointing tree.

```
Input: k, up-pointing tree (encoded as an array)

    k = 2

    indices: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
    array = [0, 0, 0, 1, 1, 2, 3, 5, 5, 5]
  
       0
      / \
     1   2
    / \   \
   3   4   5
   |      /|\ 
   6     7 8 9


Output: k-th ancestor of each node, or -1 if no such

    [-1, -1, -1, 0, 0, 0, 1, 2, 2, 2]
```

### Pseudo Code (C style)

```c
void parallel_copy_into(int dst[], int src[], int N) {
  for (int i = 0; i < N; i++) do in parallel {
    dst[i] = src[i];
  }
}

void deterministic_pointer_jumping(int arr[], int map[], int N) {
  // a deterministic version of pointer jumping,
  // working on a copy of arr, to avoid race conditions.

  int temp[N];

  for (int i = 0; i < N; i++) do in parallel {
    if (arr[i] > -1) {
      temp[i] = map[arr[i]];
    }
  }

  parallel_copy_into(arr, temp, N);
}

void each_node_kth_ancestor(int arr[], int results[], int N) {

  ///////////////////////
  // step 1: find root //
  ///////////////////////

  int root;

  // copy arr into results
  parallel_copy_into(results, arr, N);

  // one round of pointer jumping is enough
  deterministic_pointer_jumping(results, results, N);
  
  // root will be the only node pointing to itself
  for (int i = 0; i < N; i++) do in parallel {
    if (results[i] == i) {
      root = i;
    }
  }

  /////////////////////////////////////
  // step 2: make root point nowhere //
  /////////////////////////////////////

  arr[root] = -1;

  ////////////////////////
  // step 3: money time //
  ////////////////////////

  // copy arr into results
  parallel_copy_into(results, arr, N);

  while (k) {
    if (k & 0x1) {
      // pointer jumping in results based on arr
      deterministic_pointer_jumping(results, arr, N);
    }
    // pointer jumping in arr
    deterministic_pointer_jumping(arr, arr, N);
    k = k >> 1;
  }
}
```

### Explanation

My approach takes inspiration from the famous `Exponentiation by Squaring` algorithm.

```c
double fast_pow(double x, unsigned int n) {
  double result = 1.0;
  while (n) {
    if (n & 0x1) {
      result *= x;
    }
    x *= x;
    n >>= 1;
  }
  return result;
}
```

We perform regular pointer jumping in `arr` **on every iteration**.
* At iteration `i`, `arr` has links of distance `2 ** i`.

We perform *guided* pointer jumping in `results` **on some iterations**.
* We only jump if the i'th bit of k is set.
* We advance each result by a distance of `2 ** i` (using `arr`).

#### Example

Let's say `k = 43`; <br />
its binary representation is `0b101011`, <br />
accounting to the fact that `43 = 32 + 0 + 8 + 0 + 2 + 1`.

| `i` | `arr`       | `results`  |
| --- | ----------- | ---------- |
|  0  | jumps of 1  | jump += 1  |
|  1  | jumps of 2  | jump += 2  |
|  2  | jumps of 4  |            |
|  3  | jumps of 8  | jump += 8  |
|  4  | jumps of 16 |            |
|  5  | jumps of 32 | jump += 32 |

#### Correctness

Normal pointer jumping (as in class) corresponds with finding ancestors at distance of **at least** k.

However, in our case we need to find ancestors at distance of **exactly** k. <br />
Also, nodes without a k'th ancestor should point to `-1`.

In this algorithm, we start by we making the root point to `-1`. <br />
We also treat `-1` as a black whole (once a node gets to `-1`, it'll stay there forever).

The trick of selectively using powers of 2, with `-1` being a black whole, guarantees we'll indeed find **exact** k'th ancestors. 

#### Complexity

Assuming N cores.

`parallel_copy_into` takes `O(1)` time.

`deterministic_pointer_jumping` takes `O(1)` time.

Step 1 does Pointer-Jumping once, so it's `O(1)` time.

Step 2 is `O(1)` time.

Step 3 does Pointer-Jumping for every bit of k, so it's `O(log k)`.

**Total:** `O(log k)` time.