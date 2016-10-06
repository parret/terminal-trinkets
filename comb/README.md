# Comb

This one is actually pretty useful instead of just being a nifty toy. This script will allow you to pass it another cli command,
and it will parse through it and automatically create and run combinations of all parameters delimited by `?`. For example, assuming
that the script is named "comb":
```
comb echo one?two
```
will create and run the command `echo one && echo two`.


You can do this for any number of parameters and any number of delimited options. A practical example, and the original reason I wrote this,
is removing running docker containers:
```
comb docker stop?rm cont1?cont2
```
will stop and then remove containers cont1 & cont2 by running:
```
docker stop cont1 && docker stop cont2 && docker rm cont1 && docker rm cont2
```

A more complicated example:
```
comb echo one?two?three?four and five?six?seven with eight?nine?ten
```
is also completely valid and will run 36 echo commands.

It is recursive, so the order the commands run in will reflect this. For example, if the command is `comb echo 0?1 0?1 0?1`, it will have this order:
```
000
001
010
011
100
101
110
111
```

It's useful for similar commands that need to be run after eachother but only vary by a few parameters. Like docker stuff.

## Usage
Pass it a cli command and any parameters you want to run. Delimit within each parameter group with `?`