#ifndef KEYWORDS_H
#define KEYWORDS_H

#include <QString>
#include <QVector>



#define KEYWORD(w) add_kw(w)

namespace Keywords
{

static QVector<QString> all_kws;

static QString add_kw(QString nkw)
{
    all_kws.append(nkw);
    return nkw;
}

const QString KW_ROTATE = KEYWORD("rotate");        // rotate <scene|point|assignment_of_points|object> with X [degrees|radians] [around <point>]
const QString KW_FUNCTION = KEYWORD("function");    // function f(x) = x * 2 + 5
const QString KW_PLOT = KEYWORD("plot");            // plot f over (-2, 2) [continuous]
const QString KW_OVER = KEYWORD("over");            // plot f over (-2, 2) [continuous]
const QString KW_CONTINUOUS = KEYWORD("continuous");// plot f over (-2, 2) [continuous|step<cnt=0.01>]
const QString KW_LET = KEYWORD("let");              // let something = points of f over (-2, 2) [continuous|step<cnt|0.01>] or let something = point at x, y or let something = a + b + c
const QString KW_OF  = KEYWORD("of");               // let something = points of f over (-2, 2) [continuous|step<cnt|0.01>]
const QString KW_STEP = KEYWORD("step");            // plot f over (-2, 2) [continuous|step<cnt|0.01>]
const QString KW_COUNTS = KEYWORD("counts");        // plot f over (-2, 2) [continuous|step<cnt|0.01>|counts<X[points|segments]> ]
const QString KW_SET = KEYWORD("set");              // set color name, set color #RGB, set color #RRGGBB, set color #RRGGBBAA, set color R,G,B[,A], set color name,alpha
const QString KW_IN = KEYWORD("in");                // foreach p in something do ... done
const QString KW_DO = KEYWORD("do");                // foreach p in something do ... done
const QString KW_DONE = KEYWORD("done");            // foreach p in something do ... done
const QString KW_RANGE = KEYWORD("range");          // foreach p in range (0, 1) do ... done
const QString KW_SEGMENTS = KEYWORD("segments");    // plot f over (-2, 2) [continuous|step<cnt|0.01>|counts<X[points|segments]> ]
const QString KW_POINTS = KEYWORD("points");        // plot f over (-2, 2) [continuous|step<cnt|0.01>|counts<X[points|segments]> ]
const QString KW_FOR = KEYWORD("for");              // for i = 1 to 256 step 1 do ... done
const QString KW_TO = KEYWORD("to");              // for i = 1 to 256 step 1 do ... done

static QVector<QString> all()
{
    return all_kws;
}

};

#endif // KEYWORDS_H
