## Part `I - A`

> Determine whether the parentheses in a given mathematical equation are balanced.


### Pseudo Code (C style)

```c
char parenthesis_sign(char ch) {
  switch (ch) {
    case '(': return +1;
    case ')': return -1;
    default : return  0;
  }
}

bool are_parentheses_balanced(char expr[], int N) {
  int signs[N], balance[N];

  for (int i = 0; i < N; i++) do in parallel {
    signs[i] = parenthesis_sign(expr[i]);
  }

  balance = parallel_prefix_sum(signs);

  bool never_negative = true;

  for (int i = 0; i < N; i++) do in parallel {
    if (balance[i] < 0) {
      never_negative = false;
    }
  }
  
  return never_negative && balance[N - 1] == 0;
}
```

### Explanation

The classic sequential algorithm scans the input left to right, incrementing a balance on every `(`, and decrementing it on every `)`.

It's already pretty much a prefix-sum, if we treat `(` as `+1` and `)` as `-1`; We just need to check that at no point the balance is negative, and that the final balance is zero.

Here we use parallel-prefix-sum as a black box, then verify (in parallel) that at no point the balance is negative.

Note that different threads assigning `never_negative = false` in parallel is *not* a race condition (there are no reads).

#### Correctness

If the expression **is not balanced**, then **either**
* there's a `)` with no a preceeding `(`, or
* there's a `(` with no succeeding `)`.

In the first case, the balance at that `)` will be negative, and the algorithm will return false; <br />
In the second case, the *final* balance will be positive, and the algorithm will return false.

If the expression **is balanced**, then **both**
* every `(` will increase the balance by 1, but it'll come with exactly one succeeding `)` that will decrease it by 1; so the final balance will be zero.
* no `)` will come before its matcing `(`, so at no point will the balance go negative.

Hence, the algorithm will return true.

#### Complexity

In class we saw that `parallel_prefix_sum` can be done in `O(log n)` time.

My additions can be done in `O(1)` (given `N` cores).

**Hence `are_parentheses_balanced` can be run in `O(log n)` time.**

(By the way, for the same reasons, it's also `O(n)` work).

---

## Part `I - B`

> Find the number of continuous sub arrays with a given sum.

In the forum, Saar said we can assume all values are positive.

### Pseudo Code (C style)

```c
bool binary_search(int arr[], int N, int x) {
  // returns whether arr contains x
  // using classic sequential binary search
}

int continuous_sub_arr_sum(int arr[], int N, int target) {
  int arr_cumsum[N], results[N], results_cumsum[N];

  results[0] = (arr[0] == target) ? 1 : 0;

  arr_cumsum = parallel_prefix_sum(arr);

  for (int i = 1; i < N; i++) do in parallel {
    int complement = arr[i] - target;
    bool found = binary_search(arr, i, complement);
    results[i] = found ? 1 : 0;
  }

  results_cumsum = parallel_prefix_sum(results);

  return results_cumsum[N - 1];
}
```

<style>
r { color: Red }
o { color: Orange }
g { color: Green }
</style>

### Explanation

As an example, consider
* arr = [ 3, <r>2, 4, 5</r>, 1, 9, <g>8, 1, 2</g>, 6 ]
* target = 11

After prefix-sum we get
* arr_cumsum = [ <r>3</r>, 5, 9, <r>14</r>, 15, <g>24</g>, 32, 33, <g>35</g>, 41 ]

Notice <r>14 - 3</r> = 11, and <g>35 - 24</g> = 11.

We'll say that y is x's *complement*, if x - y == target.

Each element in arr_cumsum can determine whether it has a preceding complement using binary search. Note that since ∀i arr[i] > 0, arr_cumsum is strictly monotonous.

The first element is a special case (it has no preceding values), so we add a check for it.

But we still need to tackle **parallel mutual counting**!

* Letting threads increment a shared counter is a race condition.
* Protecting the counter with a mutex gives O(n) runtime.

Solution: We create an array of results. Each thread sets their result to either 0 or 1.
Afterwards, we sum the results (in parallel) in O(log n) time.

#### Complexity

Assuming N threads, `parallel_prefix_sum(arr)` takes `O(log n)` time.

The binary search takes another `O(log n)` for each thread in parallel.

The last `parallel_prefix_sum(results)` again adds `O(log n)` time.

**Total: `O(log n)` time**.

(Work is `O(n log n)`, because N threads peform logarithmic binary search).