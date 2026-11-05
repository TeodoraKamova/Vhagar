#include "Diagnostics.h"
#include <random>
#include <string>
/*
https://www.asciiart.eu/mythology/dragons
artist not mentioned

        ,     \    /      ,
       / \    )\__/(     / \
      /   \  (_\  /_)   /   \
 ____/_____\__\@  @/___/_____\____
|             |\../|              |
|              \VV/               |
|     error message may go here   |
|_________________________________|
 |    /\ /      \\       \ /\    |
 |  /   V        ))       V   \  |
 |/     `       //        '     \|
 `              V                '

 https://ascii.co.uk/art/dragons
                      _                    _
                  ,/                        \,
        _________{(                          })_________
       /.-------./\\                        //\.-------.\
      //@@@@@@@//@@\\  )                (  //@@\\@@@@@@@\\
     //@@@@@@@//@@@@>>/                  \<<@@@@\\@@@@@@@\\
    //O@O@O@O//@O@O//                      \\O@O@\\O@O@O@O\\
  //OOOOOOOO//OOOO||          \  /          ||OOOO\\OOOOOOOO\\
 //O%O%O%O%//O%O%O%\\         ))((         //%O%O%O\\%O%O%O%O\\
||%%%%%%%%//'  `%%%%\\       //  \\       //%%%%'   `\\%%%%%%%||
((%%%%%%%((      %%%%%\\    ((    ))    //%%%%%       ))%%%%%%))
 \:::' `::\\      `:::::\\   \)~~(/    //:::::'      //::' `:::/
  )'     `;)'      (`  ` \\ `<@  @>' / / '  ')      `(;'     `(
          (               \`\ )^^( /  /               )
        _                  ) \\oo/   (
       (@)                  \  `'   /                      _
       |-|\__________________\__^__<________oOo__________ (@)
       | |                                  VVV          \|-|
       |-|                                                |-|
       |_|\_____________________________________________  | |
       (@)                 / ,/ \_____/ \\ ~\/~         `\|-|
        ~             ___//^~      \____/\\               (@)
                     <<<  \     __  <____/||               ~
                               <   \ <___/||
                                  || <___//
                                  \ \/__//
                                   ~----~

                       /|                 |\
                      / | ___.--~~~--.___ | \
 ...--=.._           /  ~~___~\_   _/~___~~  \        _..=--...
~   .-=_)/==._     _ \ .(~  o~-.\ /.-~o  ~). / _  _.==\(_=-.   ~~-._
  _/.-  /         / \/\_ ~---~_-=V=-_~---~ _/\/ \      \  -.\_      -
 /_/  ./          \/\_-_~~v-/~o~) (~o~\-v~~_-_/\/       \.  \_\      ~
//    |          _-=___==/(__         __)\==___=-_       |    \\\
 ))    \          _/ _ \ X___---===---___X / _ \_       /    (( ))\
\ \_   |         /_\/_\ (( \| ` ` ' ' |/ )) /_\/_\      |   _/ /|  \
 \  \   \       ~~  / _ /\\ V  /~V~)  V //\ _ \  ~~    /   /  / |   \
  \_ \  |           \/ \\ \\^  \ )/   ^//|// \/        |  / _/  |    \
    \ \  \               '/\\^  )/   ^// |`           /  / /     |
     \ \_ |             '| (\\^ V   ^//  |`          | _/ /      |
      \  \\             '/(~~\`-___-'/) /`           //  /       |
       \_ \\ /|        '|(~~~-\_   _/)  |`          |\ _/        |
         \_/~~~~-.     '/(~~~---===~~)  |`      .-~~~~~\_       |
._      _/   _    \   '|(~~~-----~~~)  /`      /     _   \_ .-. | _._
  \    // / /\\~)  \  '|(~~~-----~~~)  |`     /   (~//\ \ \X   \|/   \
------/ \ \ )--=====----=====------------=====-----===-\_) \\---------
      \ (\_)\                                            /\_)\
       \_)\                                                /\_)
        \                                                    /

        // https://asciiart.website/index.php?art=creatures/dragons
                                  /   \
 _                        )      ((   ))     (
(@)                      /|\      ))_((     /|\
|-|                     / | \    (/\|/\)   / | \                      (@)
| | -------------------/--|-voV---\`|'/--Vov-|--\---------------------|-|
|-|                         '^`   (o o)  '^`                          | |
| |                               `\Y/'                               |-|
|-|                                                                   | |
| |                                                                   |-|
|-|                                                                   | |
| |                                                                   |-|
|_|___________________________________________________________________| |
(@)              l   /\ /         ( (       \ /\   l                `\|-|
                 l /   V           \ \       V   \ l                  (@)
                 l/                _) )_          \I
                                   `\ /'
                              Jeff Ferris



What yould we do with this guy, maybe something like. Vhagar noticed an error. Quick, fix it before
he turns around!
              __
          _.-'.-'-.__
       .-'.       '-.'-._ __.--._
-..'\,-,/..-  _         .'   \   '----._
 ). /_ _\' ( ' '.         '-  '/'-----._'-.__
 '..'     '-r   _      .-.       '-._ \
 '.\. Y .).'       ( .'  .      .\          '\'.
 .-')'|'/'-.        \)    )      '',_      _.c_.\
   .<, ,>.          |   _/\        . ',   :   : \\
  .' \_/ '.        /  .'   |          '.     .'  \)
                  / .-'    '-.        : \   _;   ||
                 / /    _     \_      '.'\ ' /   ||
                /.'   .'        \_      .|   \   \|
               / /   /      __.---'      '._  ;  ||
              /.'  _:-.____< ,_           '.\ \  ||
             // .-'     '-.__  '-'-\_      '.\/_ \|
            ( };====.===-==='        '.    .  \\: \
             \\ '._        /          :   ,'   )\_ \
              \\   '------/            \ .    /   )/
               \|        _|             )Y    |   /
                \\      \             .','   /  ,/
                 \\    _/            /     _/
                  \\   \           .'    .'
                   '| '1          /    .'
                     '. \        |:    /
                       \ |       /', .'
                        \(      ( ;z'
                         \:      \ '(_
                          \_,     '._ '-.___
                                      '-' -.\
*/

namespace diagnostics
{
    constexpr auto DRAGON_TOP1 = R"V0G0N(
                      _                    _
                  ,/                        \,
        _________{(                          })_________
       /.-------./\\                        //\.-------.\
      //@@@@@@@//@@\\  )                (  //@@\\@@@@@@@\\
     //@@@@@@@//@@@@>>/                  \<<@@@@\\@@@@@@@\\
    //O@O@O@O//@O@O//                      \\O@O@\\O@O@O@O\\
  //OOOOOOOO//OOOO||          \  /          ||OOOO\\OOOOOOOO\\
 //O%O%O%O%//O%O%O%\\         ))((         //%O%O%O\\%O%O%O%O\\
||%%%%%%%%//'  `%%%%\\       //  \\       //%%%%'   `\\%%%%%%%||
((%%%%%%%((      %%%%%\\    ((    ))    //%%%%%       ))%%%%%%))
 \:::' `::\\      `:::::\\   \)~~(/    //:::::'      //::' `:::/
  )'     `;)'      (`  ` \\ `<@  @>' / / '  ')      `(;'     `(
          (               \`\ )^^( /  /               )
        _                  ) \\oo/   (
       (@)                  \  `'   /                      _
       |-|\__________________\__^__<________oOo__________ (@)
       | |                                  VVV          \|-|)V0G0N";
    constexpr auto SIGN_WIDTH1 = sizeof("\\_____________________________________________  ") - 1;
    constexpr auto LINE_WIDTH1 = SIGN_WIDTH1 - 6; // 3 spaces padding
    constexpr auto BEGIN_PADDING1 = sizeof("       ") - 1;
    constexpr auto CENTER1 = true;
    constexpr auto DRAGON_BOTTOM1 = R"V0G0N(
       |_|\_____________________________________________  | |
       (@)                 / ,/ \_____/ \\ ~\/~         `\|-|
        ~             ___//^~      \____/\\               (@)
                     <<<  \     __  <____/||               ~
                               <   \ <___/||
                                  || <___//
                                  \ \/__//
                                   ~----~)V0G0N";

    constexpr auto DRAGON_TOP2 = R"V0G0N(
        ,     \    /      ,
       / \    )\__/(     / \
      /   \  (_\  /_)   /   \
 ____/_____\__\@  @/___/_____\____
|             |\../|              |
|              \VV/               |)V0G0N";
    constexpr auto SIGN_WIDTH2 = sizeof("|_________________________________|") - 1 - 2;
    constexpr auto LINE_WIDTH2 = SIGN_WIDTH2 - 6; // 3 spaces padding
    constexpr auto BEGIN_PADDING2 = 0;
    constexpr auto CENTER2 = true;
    constexpr auto DRAGON_BOTTOM2 = R"V0G0N(
|_________________________________|
 |    /\ /      \\       \ /\    |
 |  /   V        ))       V   \  |
 |/     `       //        '     \|
 `              V                ')V0G0N";

    constexpr auto DRAGON_TOP3 = R"V0G0N(
                       /|                 |\
                      / | ___.--~~~--.___ | \
 ...--=.._           /  ~~___~\_   _/~___~~  \        _..=--...
~   .-=_)/==._     _ \ .(~  o~-.\ /.-~o  ~). / _  _.==\(_=-.   ~~-._
  _/.-  /         / \/\_ ~---~_-=V=-_~---~ _/\/ \      \  -.\_      -
 /_/  ./          \/\_-_~~v-/~o~) (~o~\-v~~_-_/\/       \.  \_\      ~
//    |          _-=___==/(__         __)\==___=-_       |    \\\
 ))    \          _/ _ \ X___---===---___X / _ \_       /    (( ))\
\ \_   |         /_\/_\ (( \| ` ` ' ' |/ )) /_\/_\      |   _/ /|  \
 \  \   \       ~~  / _ /\\ V  /~V~)  V //\ _ \  ~~    /   /  / |   \
  \_ \  |           \/ \\ \\^  \ )/   ^//|// \/        |  / _/  |    \
    \ \  \               '/\\^  )/   ^// |`           /  / /     |
     \ \_ |             '| (\\^ V   ^//  |`          | _/ /      |
      \  \\             '/(~~\`-___-'/) /`           //  /       |
       \_ \\ /|        '|(~~~-\_   _/)  |`          |\ _/        |
         \_/~~~~-.     '/(~~~---===~~)  |`      .-~~~~~\_       |
._      _/   _    \   '|(~~~-----~~~)  /`      /     _   \_ .-. | _._
  \    // / /\\~)  \  '|(~~~-----~~~)  |`     /   (~//\ \ \X   \|/   \
------/ \ \ )--=====----=====------------=====-----===-\_) \\---------
      \ (\_)\   Dear user,                               /\_)\
       \_)\     we hope this error finds you well.         /\_)
        \                                                    /)V0G0N";
    constexpr auto SIGN_WIDTH3 =
        sizeof("---------------=====----=====------------=====-----===----------------") - 1 - 2;
    constexpr auto LINE_WIDTH3 = SIGN_WIDTH3 - 32;
    constexpr auto BEGIN_PADDING3 = 0;
    constexpr auto CENTER3 = false;
    constexpr auto DRAGON_BOTTOM3 = R"V0G0N(
    
                Best Regards,
                Team Vhagar
----------------====----=====------------=====------==----------------)V0G0N";

    constexpr auto DRAGON_TOP4 = R"V0G0N(
                                  /   \
 _                        )      ((   ))     (
(@)                      /|\      ))_((     /|\
|-|                     / | \    (/\|/\)   / | \                      (@)
| | -------------------/--|-voV---\`|'/--Vov-|--\---------------------|-|
| |                         '^`   (o o)  '^`                          | |
| |                               `\Y/'                               | |)V0G0N";
    constexpr auto SIGN_WIDTH4 =
        sizeof("___________________________________________________________________") - 1;
    constexpr auto LINE_WIDTH4 = SIGN_WIDTH4 - 18;
    constexpr auto BEGIN_PADDING4 = 0;
    constexpr auto CENTER4 = true;
    constexpr auto DRAGON_BOTTOM4 = R"V0G0N(
|_|___________________________________________________________________| |
(@)              l   /\ /         ( (       \ /\   l                `\|-|
                 l /   V           \ \       V   \ l                  (@)
                 l/                _) )_          \I
                                   `\ /'
                            art by Jeff Ferris)V0G0N";

    constexpr auto DRAGON_TOP5 = R"V0G0N(
              __
          _.-'.-'-.__
       .-'.       '-.'-._ __.--._
-..'\,-,/..-  _         .'   \   '----._
 ). /_ _\' ( ' '.         '-  '/'-----._'-.__
 '..'     '-r   _      .-.       '-._ \
 '.\. Y .).'       ( .'  .      .\          '\'.
 .-')'|'/'-.        \)    )      '',_      _.c_.\
   .<, ,>.          |   _/\        . ',   :   : \\
  .' \_/ '.        /  .'   |          '.     .'  \)
                  / .-'    '-.        : \   _;   ||
                 / /    _     \_      '.'\ ' /   ||
                /.'   .'        \_      .|   \   \|
               / /   /      __.---'      '._  ;  ||
              /.'  _:-.____< ,_           '.\ \  ||
             // .-'     '-.__  '-'-\_      '.\/_ \|
            ( };====.===-==='        '.    .  \\: \
             \\ '._        /          :   ,'   )\_ \
              \\   '------/            \ .    /   )/
               \|        _|             )Y    |   /
                \\      \             .','   /  ,/
                 \\    _/            /     _/
                  \\   \           .'    .'
                   '| '1          /    .'
                     '. \        |:    /
                       \ |       /', .'
                        \(      ( ;z'
+----------------------- \: ---- \ '(_ -------------------------+
|      Vhagar noticed     \_,     '._ '-.___                    |
|      a critical error.              '-' -.\                   |
|      Quick, fix it before she turns around!                   |
|                                                               |)V0G0N";
    constexpr auto SIGN_WIDTH5 =
        sizeof("+---------------------------------------------------------------+") - 1 - 2;
    constexpr auto LINE_WIDTH5 = SIGN_WIDTH5 - 12;
    constexpr auto BEGIN_PADDING5 = 0;
    constexpr auto CENTER5 = false;
    constexpr auto DRAGON_BOTTOM5 = R"V0G0N(
+---------------------------------------------------------------+)V0G0N";

    struct Dragon
    {
        const size_t signWidth;
        const size_t lineWidth;
        const size_t beginPadding;
        const char* dragonTop;
        const char* dragonBot;
        const char* border;
        const bool center;
    };

    constexpr struct Dragon DRAGONS[] = {
        Dragon{SIGN_WIDTH1, LINE_WIDTH1, BEGIN_PADDING1, DRAGON_TOP1, DRAGON_BOTTOM1, "| |",
               CENTER1},
        Dragon{SIGN_WIDTH2, LINE_WIDTH2, BEGIN_PADDING2, DRAGON_TOP2, DRAGON_BOTTOM2, "|", CENTER2},
        Dragon{SIGN_WIDTH3, LINE_WIDTH3, BEGIN_PADDING3, DRAGON_TOP3, DRAGON_BOTTOM3, "", CENTER3},
        Dragon{SIGN_WIDTH4, LINE_WIDTH4, BEGIN_PADDING4, DRAGON_TOP4, DRAGON_BOTTOM4, "| |",
               CENTER4},
        Dragon{SIGN_WIDTH5, LINE_WIDTH5, BEGIN_PADDING5, DRAGON_TOP5, DRAGON_BOTTOM5, "|", CENTER5},
    };

    // we are using this internally to prettify printing of vectors
    template <typename T> std::ostream& operator<<(std::ostream& os, const std::vector<T>& vec)
    {
        for (const auto& elem : vec)
            os << " - " << elem << std::endl;

        return os;
    }

    const inline Dragon& getDragon(void)
    {
        constexpr size_t NUM_DRAGONS = sizeof(DRAGONS) / sizeof(DRAGONS[0]);
        std::random_device rd;
        std::mt19937 eng(rd());
        std::uniform_int_distribution<size_t> distr(0, NUM_DRAGONS - 1);
        const auto rand = distr(eng);
        return DRAGONS[rand];
    }

    [[noreturn]] void dragon(const std::string& errstr)
    {
        const Dragon& dragon = getDragon();

        std::cerr << dragon.dragonTop;

        // print error within the sign
        for (size_t i = 0; i < errstr.size();)
        {
            const size_t length =
                (errstr.size() - i < dragon.lineWidth) ? errstr.size() - i : dragon.lineWidth;
            const std::string_view line = std::string_view(&errstr[i], length);
            const size_t padding = dragon.signWidth - (dragon.center ? length : dragon.lineWidth);
            const size_t leftPadding = padding / 2 - (line[0] == ' ' ? 1 : 0);
            const size_t rightPadding =
                padding - leftPadding + (dragon.center ? 0 : dragon.lineWidth - length);

            std::cerr << std::endl
                      << std::string(dragon.beginPadding, ' ') << dragon.border
                      << std::string(leftPadding, ' ') << line;

            const bool useMinus =
                i + length < errstr.size() && (line.back() != ' ' && errstr[i + length] != ' ');
            if (useMinus)
                std::cerr << '-';

            std::cerr << std::string(rightPadding - useMinus, ' ') << dragon.border;

            i += length;
        }

        std::cerr << dragon.dragonBot << std::endl;

        exit(EXIT_FAILURE);
    }
} // namespace diagnostics
