import numpy as np
from scipy.linalg import expm

# Parameters
beta    = 0.003
gamma   = 0.1

# Initial conditions
S0      = 100
Npop    = 101

# Timespan
tend    = 40
tnum    = 10
dt      = tend / tnum

# Create infinitesimal generator matrix
n = np.arange(Npop + 1)

L = gamma * (Npop - (n[1:] - 1))
C = - (beta * n * (Npop - n) + gamma * (Npop - n))
R = beta * (n[:-1] + 1) * (Npop - (n[:-1] + 1))

Q = np.diag(L, -1) + np.diag(C) + np.diag(R, 1)

# Solve
y0 = np.zeros(Npop + 1, dtype=np.float)
y0[S0] = 1

tspan   = np.arange(dt, tend + dt, dt)

y = []
for t in tspan:
    y.append(np.dot(expm(Q * t), y0))

y = np.array(y)

print("y: {}".format(y))
from IPython import embed; embed()
