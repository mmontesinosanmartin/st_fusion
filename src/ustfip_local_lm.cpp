// [[Rcpp::depends(RcppArmadillo)]]
#include <RcppArmadillo.h>
#include "POOL.h"
using namespace Rcpp;

// // [[Rcpp::export]]
// arma::uvec get_ngbs(int i, int w, int nrow, int ncol) {
//   // current row-col
//   int row = (int) floor(i / ncol);
//   int col = i - (ncol * row);
//   // window limits
//   int wcs = -w; if(col-w < 0) wcs = - col;
//   int wce =  w; if((col + w) > (ncol - 1)) wce = ncol - col -1;
//   int wrs = -w; if(row-w < 0) wrs = - row;
//   int wre =  w; if(row + w > nrow - 1) wre = nrow - row - 1;
//   // initialize output
//   int wln = (abs(wcs) + abs(wce) + 1) * (abs(wrs) + abs(wre) + 1);
//   arma::uvec out(wln);
//   // sliding indices
//   size_t z = 0;
//   for(int j = wrs; j <= wre; j++){
//     for(int k = wcs; k <= wce; k++){
//       out[z] = (ncol * j) + i + k;
//       z++;
//     }
//   }
//   // return output
//   return out;
// }


// [[Rcpp::export]]
arma::mat local_lm(arma::mat& x,
                   arma::mat& y,
                   arma::uvec& n,
                   arma::ivec& cdims,
                   int w) {
  // initialize output
  int npx = x.n_rows;
  int ntm = x.n_cols;
  int ncl = ntm + 1;
  arma::mat out(npx, ncl);
  out.fill(arma::datum::nan);
  // inner parameters
  int nrow = cdims(0);
  int ncol = cdims(1);
  // for each pixel
  for(int i = 0; i < npx; i++) {
    // neighboring pixels
    arma::uvec inds = get_ngbs(i, w, nrow, ncol);
    // regression data
    arma::mat yng = y.rows(inds);
    arma::mat xng = x.rows(inds);
    arma::vec xin = xng.col(n(i));
    // when available
    if(xin.is_finite()){
      arma::vec kns = arma::ones(xin.n_rows);
      // coefficients
      arma::mat xinp = join_horiz(xin, kns);
      arma::vec coef = arma::solve(xinp, yng);
      // saving
      arma::vec outi = arma::zeros(ncl);
      outi(n(i)) = coef[0];
      outi(ncl - 1) = coef[1];
      out.row(i) = outi.as_row();
    }
  }
  // return result
  return out;
}
