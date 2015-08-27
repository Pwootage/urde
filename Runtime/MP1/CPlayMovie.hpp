#ifndef __RETRO_CPLAYMOVIE_HPP__
#define __RETRO_CPLAYMOVIE_HPP__

#include "CPlayMovieBase.hpp"

namespace Retro
{
namespace MP1
{

extern const char* kMovies[];

class CPlayMovie : public CPlayMovieBase
{
public:
    enum EWhichMovie
    {
        MovieWinGame,
        MovieLoseGame
    };
private:
    EWhichMovie x14_which;
public:
    CPlayMovie(EWhichMovie which) : CPlayMovieBase("CPlayMovie", kMovies[which]), x14_which(which) {}
};

}
}

#endif // __RETRO_CPLAYMOVIE_HPP__