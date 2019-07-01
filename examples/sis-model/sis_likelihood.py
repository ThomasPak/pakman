import numpy as np
from scipy.linalg import expm
from scipy.integrate import quad, dblquad

def evaluate_SIS_likelihood(beta, gamma, Npop, dt, y):

    # It is assumed that the first element of y is at t = 0 and that all
    # subsequent observations were made at times t = dt * np.arange(len(y))

    if (type(beta) == float):
        beta = np.array(beta)

    if (type(gamma) == float):
        gamma = np.array(gamma)

    if (beta.shape != gamma.shape):
        if beta.shape == ():
            beta = np.full(gamma.shape, beta)
        elif gamma.shape == ():
            gamma = np.full(beta.shape, gamma)

    assert (beta.shape == gamma.shape), "beta and gamma should numpy arrays with the same shape"

    output_shape = beta.shape

    beta = beta.flatten()
    gamma = gamma.flatten()

    # Create infinitesimal generator matrix
    n = np.arange(Npop + 1)

    L = gamma[:,np.newaxis] * (Npop - (n[1:] - 1))[np.newaxis, :]
    C = - (beta[:, np.newaxis] * (n * (Npop - n))[np.newaxis, :] +
            gamma[:, np.newaxis] * (Npop - n)[np.newaxis, :])
    R = beta[:, np.newaxis] * ((n[:-1] + 1) * (Npop - (n[:-1] + 1)))[np.newaxis, :]

    Q = np.zeros((len(beta), Npop + 1, Npop + 1), dtype=np.float)
    Q[:, np.arange(Npop + 1), np.arange(Npop + 1)] = C
    Q[:, np.arange(Npop), np.arange(Npop) + 1] = R
    Q[:, np.arange(Npop) + 1, np.arange(Npop)] = L

    # Compute matrix exponential
    Qdt = Q * dt

    expmQdt = np.zeros(Qdt.shape)
    for i in range(len(Qdt)):
        expmQdt[i, :, :] = expm(Qdt[i, :, :])

    # Create Y matrix
    Y = np.zeros((Npop + 1, len(y) - 1), dtype=np.float)
    Y[y[:-1], np.arange(len(y) - 1)] = 1

    # Compute probability vectors
    P = np.dot(expmQdt, Y)

    # Extract relevant probabilities
    p_array = P[:, y[1:], np.arange(len(y) - 1)]

    # Compute product of probabilities
    prod_array = np.prod(p_array, axis=1)

    # Return output with appropriate output shape
    return np.reshape(prod_array, output_shape)

def evaluate_marginal_SIS_likelihood_in_beta(beta, gamma_low, gamma_high, Npop, dt, y):

    # Define integrand
    f = lambda gamma: evaluate_SIS_likelihood(beta, gamma, Npop, dt, y)

    return quad(f, gamma_low, gamma_high)

def evaluate_marginal_SIS_likelihood_in_gamma(beta_low, beta_high, gamma, Npop, dt, y):

    # Define integrand
    f = lambda beta: evaluate_SIS_likelihood(beta, gamma, Npop, dt, y)

    return quad(f, beta_low, beta_high)

def integrate_SIS_likelihood(beta_low, beta_high, gamma_low, gamma_high, Npop,
    dt, y):

    # Integrand
    f = lambda beta, gamma: evaluate_SIS_likelihood(beta, gamma, Npop, dt, y)

    return dblquad(f, gamma_low, gamma_high, lambda gamma: beta_low, lambda
        gamma: beta_high, epsabs=1e-19)

if __name__ == "__main__":

    # Prior domain
    beta_lim   = (0, 0.06)
    gamma_lim  = (0, 2)

    # Observed data
    Npop = 101
    dt = 4
    y = [100, 98, 95, 87, 74, 59, 44, 35, 39, 32, 38]

    # Integrate over prior domain
    W = integrate_SIS_likelihood(beta_lim[0], beta_lim[1], gamma_lim[0],
            gamma_lim[1], Npop, dt, y)

    # Calculated norm_W: 1.0783711147429725e-16
    norm_W = W[0]

    # Normalize likelihood and check that integration adds up to 1
    g = lambda beta, gamma: evaluate_SIS_likelihood(beta, gamma, 101, 4, y) / norm_W
    Z = dblquad(g, 0, 2, lambda gamma: 0, lambda gamma: .06)

    print("Z: {}".format(Z))
