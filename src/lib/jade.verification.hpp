/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2016 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#ifndef JADE_VERIFICATION_HPP__
#define JADE_VERIFICATION_HPP__

#include "jade.genotype_matrix.hpp"

namespace jade
{
    ///
    /// A template for a class that performs validation on various types of
    /// matrices.
    ///
    template <typename TValue>
    class basic_verification
    {
    public:
        /// The value type.
        typedef TValue value_type;

        /// The matrix type.
        typedef basic_matrix<value_type> matrix_type;

        /// The genotype matrix type.
        typedef basic_genotype_matrix<value_type> genotype_matrix_type;

        /// The epsilon value.
        static constexpr value_type epsilon = value_type(0.000001);

        ///
        /// Validates the C matrix and throws an exception if validation fails.
        /// \return True.
        ///
        static bool validate_c(
                const matrix_type & c) ///< The C matrix to validate.
        {
            if (!c.is_square())
                throw error() << "invalid C matrix size " << c.get_size_str()
                              << " is not square";

            const auto rk = c.get_height();
            if (rk < 1)
                throw error() << "invalid C matrix size " << c.get_size_str()
                              << " does not contain at least two populations";

            for (size_t y = 0; y < rk; y++)
                for (size_t x = y + 1; x < rk; x++)
                    if (std::fabs(c(y, x) - c(x, y)) > epsilon)
                        throw error()
                            << "invalid C matrix cell [" << y+1 << ","
                            << x+1 << "] (" << c(y, x)
                            << ") is not equal to symmetric cell ["
                            << x+1 << "," << y+1 << "] (" << c(x, y) << ")";


            // Create a copy of C before LAPACK replaces it with the
            // Cholesky square root and then its inverse.
            matrix_type cholesky (c);
            if (!cholesky.potrf_lower() || !cholesky.potri_lower())
                throw error("invalid C matrix is not positive semidefinite");

            return true;
        }

        ///
        /// Validates the F matrix and throws an exception if validation fails.
        /// \return True.
        ///
        static bool validate_f(
                const matrix_type & f) ///< The F matrix to validate.
        {
            const auto K = f.get_height();
            const auto J = f.get_width();

            if (K < 2)
                throw error() << "invalid F matrix size " << f.get_size_str()
                              << " does not contain at least two popualtions";

            if (J < 2)
                throw error() << "invalid F matrix size " << f.get_size_str()
                              << " does not contain at least two markers";

            for (size_t k = 0; k < K; k++)
            {
                for (size_t j = 0; j < J; j++)
                {
                    const auto f_kj = f(k, j);

                    if (f_kj >= value_type(0) && f_kj <= value_type(1))
                        continue;

                    throw error()
                        << "invalid F matrix cell [" << k+1 << "," << j+1
                        << "] (" << f_kj << ") is not between 0 and 1";
                }
            }

            return true;
        }

        ///
        /// Validates the sizes of the F and C matrices and throws an exception
        /// if validation fails.
        /// \return True.
        ///
        static bool validate_fc_sizes(
                const matrix_type & f, ///< The F matrix to validate.
                const matrix_type & c) ///< The C matrix to validate.
        {
            if (f.get_height() != c.get_height() + 1)
                throw error()
                    << "inconsistent F matrix size " << f.get_size_str()
                    << " and C matrix size " << c.get_size_str();

            return true;
        }

        ///
        /// Validates the G matrix and throws an exception if validation fails.
        /// \return True.
        ///
        static bool validate_g(
                const genotype_matrix_type & g) ///< The G matrix to validate.
        {
            if (g.get_height() < 2)
                throw error() << "invalid G matrix size " << g.get_size_str()
                              << " does not contain at least two individuals";

            if (g.get_width() < 2)
                throw error() << "invalid G matrix size " << g.get_size_str()
                              << " does not contain at least two markers";

            return true;
        }

        ///
        /// Validates the sizes of the G and F matrices and throws an exception
        /// if validation fails.
        /// \return True.
        ///
        static bool validate_gf_sizes(
                const genotype_matrix_type & g, ///< The G matrix to validate.
                const matrix_type &          f) ///< The F matrix to validate.
        {
            if (g.get_width() != f.get_width())
                throw error()
                    << "inconsistent G matrix size " << g.get_size_str()
                    << " and F matrix size " << f.get_size_str();

            return true;
        }

        ///
        /// Validates the sizes of the G and Q matrices and throws an exception
        /// if validation fails.
        /// \return True.
        ///
        static bool validate_gq_sizes(
                const genotype_matrix_type & g, ///< The G matrix to validate.
                const matrix_type &          q) ///< The Q matrix to validate.
        {
            if (g.get_height() != q.get_height())
                throw error()
                    << "inconsistent G matrix size " << g.get_size_str()
                    << " and Q matrix size " << q.get_size_str();

            return true;
        }

        ///
        /// Validates the sizes of the G, Q, and F matrices and throws an
        /// exception if validation fails.
        /// \return True.
        ///
        static bool validate_gqf_sizes(
                const genotype_matrix_type & g, ///< The G matrix to validate.
                const matrix_type &          q, ///< The Q matrix to validate.
                const matrix_type &          f) ///< The F matrix to validate.
        {
            return validate_gq_sizes(g, q)
                && validate_gf_sizes(g, f)
                && validate_qf_sizes(q, f);
        }

        ///
        /// Validates the Q matrix and throws an exception if validation fails.
        /// \return True.
        ///
        static bool validate_q(
                const matrix_type & q) ///< The Q matrix to validate.
        {
            const auto I = q.get_height();
            const auto K = q.get_width();

            if (I < 2)
                throw error() << "invalid Q matrix size " << q.get_size_str()
                              << " does not contain at least two individuals";

            if (K < 2)
                throw error() << "invalid Q matrix size " << q.get_size_str()
                              << " does not contain at least two populations";

            for (size_t i = 0; i < I; i++)
            {
                auto sum = value_type(0);
                auto min = q(i, 0);
                auto max = q(i, 0);

                for (size_t k = 0; k < K; k++)
                {
                    const auto q_ik = q(i, k);

                    if (q_ik >= value_type(0) && q_ik <= value_type(1))
                    {
                        sum += q_ik;
                        min = std::min(min, q_ik);
                        max = std::max(max, q_ik);
                        continue;
                    }

                    throw error()
                        << "invalid Q matrix cell [" << i+1 << "," << k+1
                        << "] (" << q_ik << ") is not between 0 and 1";
                }

                if (std::fabs(sum - value_type(1)) < epsilon)
                {
                    if (std::fabs(max - min) > epsilon)
                        continue;

                    throw error() << "invalid Q matrix row " << i+1
                                  << " has only one value (" << min << ")";
                }

                throw error() << "invalid Q matrix row " << i+1
                              << " does not sum to 1 (" << sum << ")";
            }

            return true;
        }

        ///
        /// Validates the sizes of the Q and F matrices and throws an exception
        /// if validation fails.
        /// \return True.
        ///
        static bool validate_qf_sizes(
                const matrix_type & q, ///< The Q matrix to validate.
                const matrix_type & f) ///< The F matrix to validate.
        {
            if (q.get_width() != f.get_height())
                throw error()
                    << "inconsistent Q matrix size " << q.get_size_str()
                    << " and F matrix size " << f.get_size_str();

            return true;
        }
    };
}

#endif // JADE_VERIFICATION_HPP__
