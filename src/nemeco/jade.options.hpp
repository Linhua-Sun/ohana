/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2016 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#ifndef JADE_OPTIONS_HPP__
#define JADE_OPTIONS_HPP__

#include "jade.args.hpp"

namespace jade
{
    ///
    /// A template for a class that reads the command-line arguments.
    ///
    template <typename TValue>
    class basic_options
    {
    public:
        /// The value type.
        typedef TValue value_type;

        /// The value assigned with no --epsilon option.
        static constexpr auto no_epsilon =
            std::numeric_limits<value_type>::quiet_NaN();

        /// The value assigned with no --max-iterations option.
        static constexpr auto no_iterations =
            std::numeric_limits<size_t>::max();

        /// The value assigned with no --max-time option.
        static constexpr auto no_time =
            std::numeric_limits<double>::quiet_NaN();

        ///
        /// Initializes a new instance of the class.
        ///
        explicit basic_options(
                args & a) ///< The command-line arguments.
            : _cin            (a.read<std::string>("--cin", "-ci"))
            , _cout           (a.read<std::string>("--cout", "-co"))
            , _epsilon        (a.read("--epsilon", "-e", no_epsilon))
            , _max_iterations (a.read("--max-iterations", "-mi", no_iterations))
            , _max_time       (a.read("--max-time", "-mt", no_time))
            , _num_threads    (a.read("--num-threads", "-nt", size_t(1)))
            , _tin            (a.read<std::string>("--tin", "-ti"))
            , _tout           (a.read<std::string>("--tout", "-to"))
        {
            if (is_epsilon_specified() && _epsilon < value_type(0))
                throw error()
                      << "invalid value for --epsilon option: " << _epsilon;

            if (is_max_time_specified() && _max_time < 0.0)
                throw error()
                      << "invalid value for --max-time option: " << _max_time;

            if (_num_threads == 0)
                throw error()
                      << "invalid number of threads specified for --num-threads"
                      << "option: " << _num_threads;

            if (is_max_iterations_specified() && _max_iterations == 0)
                throw error()
                      << "invalid number of iterations for --max-iterations "
                      << "option: " << _max_iterations;

            if (!is_tin_specified() && is_tout_specified())
                throw error("invalid specification of --tout option "
                            "without --tin option");

            if (is_cin_specified() && is_tin_specified())
                throw error("invalid specification of --cin option "
                            "with --tin option");
        }

        ///
        /// \return The C input matrix path, if specified.
        ///
        inline const std::string & get_cin() const
        {
            assert(is_cin_specified());
            return _cin;
        }

        ///
        /// \return The C output matrix path, if specified.
        ///
        inline const std::string & get_cout() const
        {
            assert(is_cout_specified());
            return _cout;
        }

        ///
        /// \return The epsilon value.
        ///
        inline value_type get_epsilon() const
        {
            assert(is_epsilon_specified());
            return _epsilon;
        }

        ///
        /// \return The maximum number of iterations.
        ///
        inline size_t get_max_iterations() const
        {
            assert(is_max_iterations_specified());
            return _max_iterations;
        }

        ///
        /// \return The maximum amount of time to execute.
        ///
        inline double get_max_time() const
        {
            assert(is_max_time_specified());
            return _max_time;
        }

        ///
        /// \return The number of threads for the CBLAS/LAPACK libraries.
        ///
        inline size_t get_num_threads() const
        {
            return _num_threads;
        }

        ///
        /// \return The T input tree path, if specified.
        ///
        inline const std::string & get_tin() const
        {
            assert(is_tin_specified());
            return _tin;
        }

        ///
        /// \return The T output tree path, if specified.
        ///
        inline const std::string & get_tout() const
        {
            assert(is_tout_specified());
            return _tout;
        }

        ///
        /// \return True if the C input matrix was specified.
        ///
        inline bool is_cin_specified() const
        {
            return !_cin.empty();
        }

        ///
        /// \return True if the C output matrix was specified.
        ///
        inline bool is_cout_specified() const
        {
            return !_cout.empty();
        }

        ///
        /// \return True if the epsilon option was specified.
        ///
        inline bool is_epsilon_specified() const
        {
            return !std::isnan(_epsilon);
        }

        ///
        /// \return True if the maximum iterations option was specified.
        ///
        inline bool is_max_iterations_specified() const
        {
            return _max_iterations != no_iterations;
        }

        ///
        /// \return True if the maximum time option was specified.
        ///
        inline bool is_max_time_specified() const
        {
            return !std::isnan(_max_time);
        }

        ///
        /// \return True if the T input tree was specified.
        ///
        inline bool is_tin_specified() const
        {
            return !_tin.empty();
        }

        ///
        /// \return True if the T output tree was specified.
        ///
        inline bool is_tout_specified() const
        {
            return !_tout.empty();
        }

    private:
        std::string _cin;
        std::string _cout;
        value_type  _epsilon;
        size_t      _max_iterations;
        double      _max_time;
        size_t      _num_threads;
        std::string _tin;
        std::string _tout;
    };
}

#endif // JADE_OPTIONS_HPP__
