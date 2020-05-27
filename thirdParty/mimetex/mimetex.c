/****************************************************************************
 *
 * Copyright(c) 2002-2017, John Forkosh Associates, Inc. All rights reserved.
 *           http://www.forkosh.com   mailto: john@forkosh.com
 * --------------------------------------------------------------------------
 * This file is part of mimeTeX, which is free software. You may redistribute
 * and/or modify it under the terms of the GNU General Public License,
 * version 3 or later, as published by the Free Software Foundation.
 *      MimeTeX is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY, not even the implied warranty of MERCHANTABILITY.
 * See the GNU General Public License for specific details.
 *      By using mimeTeX, you warrant that you have read, understood and
 * agreed to these terms and conditions, and that you possess the legal
 * right and ability to enter into this agreement and to use mimeTeX
 * in accordance with it.
 *      Your mimetex.zip distribution file should contain the file COPYING,
 * an ascii text copy of the GNU General Public License, version 3.
 * If not, point your browser to  http://www.gnu.org/licenses/
 * or write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330,  Boston, MA 02111-1307 USA.
 * --------------------------------------------------------------------------
 *
 * Purpose:   o	MimeTeX, licensed under the gpl, lets you easily embed
 *		LaTeX math in your html pages.  It parses a LaTeX math
 *		expression and immediately emits the corresponding gif
 *		image, rather than the usual TeX dvi.  And mimeTeX is an
 *		entirely separate little program that doesn't use TeX or
 *		its fonts in any way.  It's just one cgi that you put in
 *		your site's cgi-bin/ directory, with no other dependencies.
 *		     So mimeTeX is very easy to install.  And it's equally
 *		easy to use.  Just place an html <img> tag in your document
 *		wherever you want to see the corresponding LaTeX expression.
 *		For example,
 *		 <img src="../cgi-bin/mimetex.cgi?\int_{-\infty}^xe^{-t^2}dt"
 *		  alt="" border=0 align=middle>
 *		immediately generates the corresponding gif image on-the-fly,
 *		displaying the rendered expression wherever you put that
 *		<img> tag.
 *		     MimeTeX doesn't need intermediate dvi-to-gif conversion,
 *		and it doesn't clutter up your filesystem with separate
 *		little gif files for each converted expression.
 *		But image caching is available by using mimeTeX's
 *		-DCACHEPATH=\"path/\" compile option (see below).
 *		There's also no inherent need to repeatedly write the
 *		cumbersome <img> tag illustrated above.  You can write
 *		your own custom tags, or write a wrapper script around
 *		mimeTeX to simplify the notation.
 *		     Further discussion about mimeTeX's features and
 *		usage is available on its homepage,
 *		  http://www.forkosh.com/mimetex.html
 *		and similarly in mimetex.html included with your mimetex.zip
 *		distribution file. (Note: http://www.forkosh.com/mimetex.html
 *		is a "quickstart" version of the the full mimetex.html manual
 *		included in your mimetex.zip distribution file.)
 *
 * Functions:	The following "table of contents" lists each function
 *		comprising mimeTeX in the order it appears in this file.
 *		See individual function entry points for specific comments
 *		about its purpose, calling sequence, side effects, etc.
 *		(All these functions eventually belong in several
 *		different modules, possibly along the lines suggested
 *		by the divisions below.  But until the best decomposition
 *		becomes clear, it seems better to keep mimetex.c
 *		neatly together, avoiding a bad decomposition that
 *		becomes permanent by default.)
 *		===================== Raster Functions ======================
 *	PART2	--- raster constructor functions ---
 *		new_raster(width,height,pixsz)   allocation (and constructor)
 *		new_subraster(width,height,pixsz)allocation (and constructor)
 *		new_chardef()                         allocate chardef struct
 *		delete_raster(rp)        deallocate raster (rp =  raster ptr)
 *		delete_subraster(sp)  deallocate subraster (sp=subraster ptr)
 *		delete_chardef(cp)      deallocate chardef (cp = chardef ptr)
 *		--- primitive (sub)raster functions ---
 *		rastcpy(rp)                           allocate new copy of rp
 *		subrastcpy(sp)                        allocate new copy of sp
 *		rastrot(rp)         new raster rotated right 90 degrees to rp
 *		rastrot3d(rp,axis,theta)new rast rotated around axis by theta
 *		rastmag(rp,magstep)   new raster magnified by "magstep" to rp
 *		bytemapmag(bytemap,width,height,magstep)      magnify bytemap
 *		rastref(rp,axis)    new raster reflected (axis 1=horz,2=vert)
 *		rastput(target,source,top,left,isopaque)  overlay src on trgt
 *		rastcompose(sp1,sp2,offset2,isalign,isfree) sp2 on top of sp1
 *		rastcat(sp1,sp2,isfree)                  concatanate sp1||sp2
 *		rastack(sp1,sp2,base,space,iscenter,isfree)stack sp2 atop sp1
 *		rastile(tiles,ntiles)      create composite raster from tiles
 *		rastsmash(sp1,sp2,xmin,ymin)      calc #smash pixels sp1||sp2
 *		rastsmashcheck(term)         check if term is "safe" to smash
 *		--- raster "drawing" functions ---
 *		accent_subraster(accent,width,height,direction,pixsz)\hat\vec
 *		arrow_subraster(width,height,drctn,isBig)    left/right arrow
 *		uparrow_subraster(width,height,drctn,isBig)     up/down arrow
 *		rule_raster(rp,top,left,width,height,type)    draw rule in rp
 *		line_raster(rp,row0,col0,row1,col1,thickness) draw line in rp
 *		line_recurse(rp,row0,col0,row1,col1,thickness)   recurse line
 *		circle_raster(rp,row0,col0,row1,col1,thickness,quads) ellipse
 *		circle_recurse(rp,row0,col0,row1,col1,thickness,theta0,theta1)
 *		bezier_raster(rp,r0,c0,r1,c1,rt,ct)   draw bezier recursively
 *		border_raster(rp,ntop,nbot,isline,isfree)put border around rp
 *		backspace_raster(rp,nback,pback,minspace,isfree)    neg space
 *		--- raster (and chardef) output (and one input) functions ---
 *		type_raster(rp,fp)       emit ascii dump of rp on file ptr fp
 *		type_bytemap(bp,grayscale,width,height,fp) dump bytemap on fp
 *		xbitmap_raster(rp,fp)           emit mime xbitmap of rp on fp
 *		type_pbmpgm(rp,ptype,file)     pbm or pgm image of rp to file
 *		read_pbm(fp,sf)     read pbm from file (or pipe) opened on fp
 *		cstruct_chardef(cp,fp,col1)         emit C struct of cp on fp
 *		cstruct_raster(rp,fp,col1)          emit C struct of rp on fp
 *		hex_bitmap(rp,fp,col1,isstr)emit hex dump of rp->pixmap on fp
 *		--- ancillary output functions ---
 *		emit_string(fp,col1,string,comment) emit string and C comment
 *		gftobitmap(rp)        convert .gf-like pixmap to bitmap image
 *		====================== Font Functions =======================
 *		--- font lookup functions ---
 *		get_symdef(symbol)              return mathchardef for symbol
 *		get_ligature(expr,family)  return symtable index for ligature
 *		get_chardef(symdef,size)       return chardef for symdef,size
 *		get_charsubraster(symdef,size)  wrap subraster around chardef
 *		get_symsubraster(symbol,size)    returns subraster for symbol
 *		--- ancillary font functions ---
 *		get_baseline(gfdata)       determine baseline (in our coords)
 *		get_delim(symbol,height,family) delim just larger than height
 *		make_delim(symbol,height) construct delim exactly height size
 *		================= Tokenize/Parse Functions ==================
 *		texchar(expression,chartoken)  retruns next char or \sequence
 *		texsubexpr(expr,subexpr,maxsubsz,left,right,isescape,isdelim)
 *		texleft(expr,subexpr,maxsubsz,ldelim,rdelim)   \left...\right
 *		texscripts(expression,subscript,superscript,which)get scripts
 *		--- ancillary parse functions ---
 *		isbrace(expression,braces,isescape)   check for leading brace
 *		preamble(expression,size,subexpr)              parse preamble
 *		mimeprep(expression) preprocessor converts \left( to \(, etc.
 *		strchange(nfirst,from,to)   change nfirst chars of from to to
 *		strreplace(string,from,to,nreplace)  change from to to in str
 *		strwstr(string,substr,white,sublen)     find substr in string
 *		strdetex(s,mode)    replace math chars like \^_{} for display
 *		strtexchr(string,texchr)                find texchr in string
 *		findbraces(expression,command)    find opening { or closing }
 *		strpspn(s,reject,segment)     non-() chars of s not in reject
 *		isstrstr(string,snippets,iscase)  are any snippets in string?
 *		isnumeric(s)                     determine if s is an integer
 *		evalterm(store,term)     evaluate numeric value of expression
 *		getstore(store,identifier)return value corresponding to ident
 *		getdirective(string,directive,iscase,isvalid,nargs,args) \dir
 *		strpspn(s,reject,segment) non-{[()]} chars of s not in reject
 *		strqspn(s,q,isunescape) find matching " or ' in quoted string
 *		unescape_url(url,isescape), x2c(what)   xlate %xx url-encoded
 *	PART3	=========== Rasterize an Expression (recursively) ===========
 *		--- here's the primary entry point for all of mimeTeX ---
 *		rasterize(expression,size)     parse and rasterize expression
 *		--- explicitly called handlers that rasterize... ---
 *		rastparen(subexpr,size,basesp)          parenthesized subexpr
 *		rastlimits(expression,size,basesp)    dispatch super/sub call
 *		rastscripts(expression,size,basesp) super/subscripted exprssn
 *		rastdispmath(expression,size,sp)      scripts for displaymath
 *		--- table-driven handlers that rasterize... ---
 *		rastleft(expression,size,basesp,ildelim,arg2,arg3)\left\right
 *		rastright(expression,size,basesp,ildelim,arg2,arg3) ...\right
 *		rastmiddle(expression,size,basesp,arg1,arg2,arg3)     \middle
 *		rastflags(expression,size,basesp,flag,value,arg3)    set flag
 *		rastspace(expression,size,basesp,width,isfill,isheight)\,\:\;
 *		rastnewline(expression,size,basesp,arg1,arg2,arg3)         \\
 *		rastarrow(expression,size,basesp,width,height,drctn) \longarr
 *		rastuparrow(expression,size,basesp,width,height,drctn)up/down
 *		rastoverlay(expression,size,basesp,overlay,arg2,arg3)    \not
 *		rastfrac(expression,size,basesp,isfrac,arg2,arg3) \frac \atop
 *		rastackrel(expression,size,basesp,base,arg2,arg3)   \stackrel
 *		rastmathfunc(expression,size,basesp,base,arg2,arg3) \lim,\etc
 *		rastsqrt(expression,size,basesp,arg1,arg2,arg3)         \sqrt
 *		rastaccent(expression,size,basesp,accent,isabove,isscript)
 *		rastfont(expression,size,basesp,font,arg2,arg3) \cal{},\scr{}
 *		rastbegin(expression,size,basesp,arg1,arg2,arg3)     \begin{}
 *		rastarray(expression,size,basesp,arg1,arg2,arg3)       \array
 *		rastpicture(expression,size,basesp,arg1,arg2,arg3)   \picture
 *		rastline(expression,size,basesp,arg1,arg2,arg3)         \line
 *		rastrule(expression,size,basesp,arg1,arg2,arg3)         \rule
 *		rastcircle(expression,size,basesp,arg1,arg2,arg3)     \circle
 *		rastbezier(expression,size,basesp,arg1,arg2,arg3)     \bezier
 *		rastraise(expression,size,basesp,arg1,arg2,arg3)    \raisebox
 *		rastrotate(expression,size,basesp,arg1,arg2,arg3)  \rotatebox
 *		rastmagnify(expression,size,basesp,arg1,arg2,arg3)   \magnify
 *		rastreflect(expression,size,basesp,arg1,arg2,arg3)\reflectbox
 *		rastfbox(expression,size,basesp,arg1,arg2,arg3)         \fbox
 *		rastovalbox(expression,size,basesp,arg1,arg2,arg3)   \ovalbox
 *		rastinput(expression,size,basesp,arg1,arg2,arg3)       \input
 *		rastcounter(expression,size,basesp,arg1,arg2,arg3)   \counter
 *		rasteval(expression,size,basesp,arg1,arg2,arg3)         \eval
 *		rastmathtex(expression,size,basesp,arg1,arg2,arg3)   \mathtex
 *		rasttoday(expression,size,basesp,arg1,arg2,arg3)       \today
 *		rastcalendar(expression,size,basesp,arg1,arg2,arg3) \calendar
 *		rastenviron(expression,size,basesp,arg1,arg2,arg3)   \environ
 *		rastmessage(expression,size,basesp,arg1,arg2,arg3)   \message
 *		rastnoop(expression,size,basesp,arg1,arg2,arg3) flush \escape
 *		--- helper functions for handlers ---
 *		rastopenfile(filename,mode)      opens filename[.tex] in mode
 *		rasteditfilename(filename)       edit filename (for security)
 *		rastreadfile(filename,islock,tag,value)   read <tag>...</tag>
 *		rastwritefile(filename,tag,value,isstrict)write<tag>...</tag>
 *		calendar(year,month,day)    formats one-month calendar string
 *		timestamp(tzdelta,ifmt)              formats timestamp string
 *		tzadjust(tzdelta,year,month,day,hour)        adjust date/time
 *		daynumber(year,month,day)     #days since Monday, Jan 1, 1973
 *		strwrap(s,linelen,tablen)insert \n's and spaces to wrap lines
 *		strnlower(s,n)        lowercase the first n chars of string s
 *		urlprune(url,n)  http://abc.def.ghi.com/etc-->abc.def.ghi.com
 *		urlncmp(url1,url2,n)   compares topmost n levels of two url's
 *		dbltoa(d,npts)                double to comma-separated ascii
 *		rotmatrix(axis,theta) rotation matrix, theta degs around axis
 *		matmult(mat,vec)        returns result of mat(rix) x vec(tor)
 *		=== Anti-alias completed raster (lowpass) or symbols (ss) ===
 *		aalowpass(rp,bytemap,grayscale)     lowpass grayscale bytemap
 *		aapnm(rp,bytemap,grayscale)       lowpass based on pnmalias.c
 *		aapnmlookup(rp,bytemap,grayscale)  aapnm based on aagridnum()
 *		aapatterns(rp,irow,icol,gridnum,patternum,grayscale) call 19,
 *		aapattern1124(rp,irow,icol,gridnum,grayscale)antialias pattrn
 *		aapattern19(rp,irow,icol,gridnum,grayscale) antialias pattern
 *		aapattern20(rp,irow,icol,gridnum,grayscale) antialias pattern
 *		aapattern39(rp,irow,icol,gridnum,grayscale) antialias pattern
 *		aafollowline(rp,irow,icol,direction)       looks for a "turn"
 *		aagridnum(rp,irow,icol)             calculates gridnum, 0-511
 *		aapatternnum(gridnum)    looks up pattern#, 1-51, for gridnum
 *		aalookup(gridnum)     table lookup for all possible 3x3 grids
 *		aalowpasslookup(rp,bytemap,grayscale)   driver for aalookup()
 *		aasupsamp(rp,aa,sf,grayscale)             or by supersampling
 *		imgsupsamp(rp,sf,grayscale)supersampling to shrink rp->pixmap
 *		ssweights(width,height,maxwt,wts)build canonical ss wt matrix
 *		aacolormap(bytemap,nbytes,colors,colormap)make colors,colormap
 *		aaweights(width,height)      builds "canonical" weight matrix
 *		aawtpixel(image,ipixel,weights,rotate) weight image at ipixel
 *		=== miscellaneous ===
 *		mimetexsetmsg(newmsglevel,newmsgfp)    set msglevel and msgfp
 *	PART1	========================== Driver ===========================
 *		main(argc,argv) parses math expression and emits mime xbitmap
 *		CreateGifFromEq(expression,gifFileName)  entry pt for win dll
 *		ismonth(month)          is month current month ("jan"-"dec")?
 *		logger(fp,msglevel,logvars)        logs environment variables
 *		emitcache(cachefile,maxage,valign,isbuffer)    emit cachefile
 *		readcachefile(cachefile,buffer)    read cachefile into buffer
 *		advertisement(expression,mode)  wrap expression in ad message
 *		crc16(s)                               16-bit crc of string s
 *		md5str(instr)                      md5 hash library functions
 *		GetPixel(x,y)           callback function for gifsave library
 *
 * Source:	mimetex.c  (needs mimetex.h and texfonts.h to compile,
 *		and also needs gifsave.c when compiled with -DAA or -DGIF)
 *
 * --------------------------------------------------------------------------
 * Notes      o	See individual function entry points for specific comments
 *		about the purpose, calling sequence, side effects, etc
 *		of each mimeTeX function listed above.
 *	      o	See bottom of file for main() driver (and "friends"),
 *		and compile as
 *		   cc -DAA mimetex.c gifsave.c -lm -o mimetex.cgi
 *		to produce an executable that emits gif images with
 *		anti-aliasing (see Notes below).  You may also compile
 *		   cc -DGIF mimetex.c gifsave.c -lm -o mimetex.cgi
 *		to produce an executable that emits gif images without
 *		anti-aliasing.  Alternatively, compile mimeTeX as
 *		   cc -DXBITMAP mimetex.c -lm -o mimetex.cgi
 *		to produce an executable that just emits mime xbitmaps.
 *		In either case you'll need mimetex.h and texfonts.h,
 *		and with -DAA or -DGIF you'll also need gifsave.c
 *	      o	The font information in texfonts.h was produced by multiple
 *		runs of gfuntype, one run per struct (i.e., one run per font
 *		family at a particular size).  Compile gfuntype as
 *		   cc gfuntype.c mimetex.c -lm -o gfuntype
 *		See gfuntype.c, and also mimetex.html#fonts, for details.
 *	      o	For gif images, the gifsave.c library by Sverre H. Huseby
 *		<http://shh.thathost.com> slightly modified by me to allow
 *		(a)sending output to stdout or returning it in memory,
 *		and (b)specifying a transparent background color index,
 *		is included with mimeTeX, and it's documented in
 *		mimetex.html#gifsave
 *	      o	MimeTeX's principal reusable function is rasterize(),
 *		which takes a string like "f(x)=\int_{-\infty}^xe^{-t^2}dt"
 *		and returns a (sub)raster representing it as a bit or bytemap.
 *		Your application can do anything it likes with this pixel map.
 *		MimeTeX just outputs it, either as a mime xbitmap or as a gif.
 *		See  mimetex.html#makeraster  for further discussion
 *		and examples.
 *	      o	File mimetex.c also contains library functions implementing
 *		a raster datatype, functions to manipulate rasterized .mf
 *		fonts (see gfuntype.c which rasterizes .mf fonts), functions
 *		to parse LaTeX expressions, etc.  As already mentioned,
 *		a complete list of mimetex.c functions is above.  See their
 *		individual entry points below for further comments.
 *		   As also mentioned, these functions eventually belong in
 *		several different modules, possibly along the lines suggested
 *		by the divisions above.  But until the best decomposition
 *		becomes clear, it seems better to keep mimetex.c
 *		neatly together, avoiding a bad decomposition that
 *		becomes permanent by default.
 *	      o	Optional compile-line -D defined symbols are documented
 *		in mimetex.html#options .  They include (additional -D
 *		switches are discussed at mimetex.html#options)...
 *		-DAA
 *		    Turns on gif anti-aliasing with default values
 *		    (CENTERWT=32, ADJACENTWT=3, CORNERWT=1)
 *		    for the following anti-aliasing parameters...
 *		-DCENTERWT=n
 *		-DADJACENTWT=j
 *		-DCORNERWT=k
 *			*** Note: Ignore these three switches because
 *			*** mimeTeX's current anti-aliasing algorithm
 *			*** no longer uses them (as of version 1.60).
 *		    MimeTeX currently provides a lowpass filtering
 *		    algorithm for anti-aliasing, which is applied to the
 *		    existing set of bitmap fonts.  This lowpass filter
 *		    applies default weights
 *				1   2   1
 *				2   8   2
 *				1   2   1
 *		    to neighboring pixels. The defaults weights are
 *		    CENTERWT=8, ADJACENTWT=2 and CORNERWT=1,
 *		    which you can adjust to control anti-aliasing.
 *		    Lower CENTERWT values will blur/spread out lines
 *		    while higher values will tend to sharpen lines.
 *		    Experimentation is recommended to determine
 *		    what value works best for you.
 *		-DCACHEPATH=\"path/\"
 *		    This option saves each rendered image to a file
 *		    in directory  path/  which mimeTeX reads rather than
 *		    re-rendering the same image every time it's given
 *		    the same LaTeX expression.  Sometimes mimeTeX disables
 *		    caching, e.g., expressions containing \input{ } are
 *		    re-rendered since the contents of the inputted file
 *		    may have changed.  If compiled without -DCACHEPATH
 *		    mimeTeX always re-renders expressions.  This usually
 *		    isn't too cpu intensive, but if you have unusually
 *		    high hit rates then image caching may be helpful.
 *			The  path/  is relative to mimetex.cgi, and must
 *		    be writable by it.  Files created under  path/  are
 *		    named filename.gif, where filename is the 32-character
 *		    MD5 hash of the LaTeX expression.
 *		-DDEFAULTSIZE=n
 *		    MimeTeX currently has eight font sizes numbered 0-7,
 *		    and always starts in DEFAULTSIZE whose default value
 *		    is 3 (corresponding to \large). Specify -DDEFAULTSIZE=4
 *		    on the compile line if you prefer mimeTeX to start in
 *		    larger default size 4 (corresponding to \Large), etc.
 *		-DDISPLAYSIZE=n
 *		    By default, operator limits like \int_a^b are rendered
 *		    \textstyle at font sizes \normalsize and smaller,
 *		    and rendered \displaystyle at font sizes \large and
 *		    larger.  This default corresponds to -DDISPLAYSIZE=3,
 *		    which you can adjust; e.g., -DDISPLAYSIZE=0 always
 *		    defaults to \displaystyle, and 99 (or any large number)
 *		    always defaults to \textstyle.  Note that explicit
 *		    \textstyle, \displaystyle, \limits or \nolimits
 *		    directives in an expression always override
 *		    the DISPLAYSIZE default.
 *		-DERRORSTATUS=n
 *		    The default, 0, means mimeTeX always exits with status 0,
 *		    regardless of whether or not it detects error(s) while
 *		    trying to render your expression.  Specify any non-zero
 *		    value (typically -1) if you write a script/plugin for
 *		    mimeTeX that traps non-zero exit statuses.  MimeTeX then
 *		    exits with its own non-zero status when it detects an
 *		    error it can identify, or with your ERRORSTATUS value
 *		    for errors it can't specifically identify.
 *		-DREFERER=\"domain\"   -or-
 *		-DREFERER=\"domain1,domain2,etc\"
 *		    Blocks mimeTeX requests from unauthorized domains that
 *		    may be using your server's mimetex.cgi without permission.
 *		    If REFERER is defined, mimeTeX checks for the environment
 *		    variable HTTP_REFERER and, if it exists, performs a
 *		    case-insensitive test to make sure it contains 'domain'
 *		    as a substring.  If given several 'domain's (second form)
 *		    then HTTP_REFERER must contain either 'domain1' or
 *		    'domain2', etc, as a (case-insensitive) substring.
 *		    If HTTP_REFERER fails to contain a substring matching
 *		    any of these domain(s), mimeTeX emits an error message
 *		    image corresponding to the expression specified by
 *		    the  invalid_referer_msg  string defined in main().
 *		    Note: if HTTP_REFERER is not an environment variable,
 *		    mimeTeX correctly generates the requested expression
 *		    (i.e., no referer error).
 *		-DWARNINGS=n  -or-
 *		-DNOWARNINGS
 *		    If an expression submitted to mimeTeX contains an
 *		    unrecognzied escape sequence, e.g., "y=x+\abc+1", then
 *		    mimeTeX generates a gif image containing an embedded
 *		    warning in the form "y=x+[\abc?]+1".  If you want these
 *		    warnings suppressed, -DWARNINGS=0 or -DNOWARNINGS tells
 *		    mimeTeX to ignore unrecognized symbols, and the rendered
 *		    image is "y=x++1" instead.
 *		-DWHITE
 *		    MimeTeX usually renders black symbols on a white
 *		    background.  This option renders white symbols on
 *		    a black background instead.
 * --------------------------------------------------------------------------
 * Revision History:
 * 09/18/02	J.Forkosh	Installation.
 * 12/11/02	J.Forkosh	Version 1.00 released.
 * 07/04/03	J.Forkosh	Version 1.01 released.
 * 10/17/03	J.Forkosh	Version 1.20 released.
 * 12/21/03	J.Forkosh	Version 1.30 released.
 * 02/01/04	J.Forkosh	Version 1.40 released.
 * 10/02/04	J.Forkosh	Version 1.50 released.
 * 11/30/04	J.Forkosh	Version 1.60 released.
 * 10/11/05	J.Forkosh	Version 1.64 released.
 * 11/30/06	J.Forkosh	Version 1.65 released.
 * 09/06/08	J.Forkosh	Version 1.70 released.
 * 03/23/09	J.Forkosh	Version 1.71 released.
 * 11/18/09	J.Forkosh	Version 1.72 released.
 * 11/15/11	J.Forkosh	Version 1.73 released.
 * 02/15/12	J.Forkosh	Version 1.74 released.
 * 12/28/16	J.Forkosh	Version 1.75 released.
 * 06/10/17	J.Forkosh	Most recent revision (also see REVISIONDATE)
 * See  http://www.forkosh.com/mimetexchangelog.html  for further details.
 *
 ****************************************************************************/

/* -------------------------------------------------------------------------
Program id
-------------------------------------------------------------------------- */
#define	VERSION "1.75"			/* mimeTeX version number */
#define REVISIONDATE "10 June 2017"	/* date of most recent revision */
#define COPYRIGHTTEXT "Copyright(c) 2002-2017, John Forkosh Associates, Inc"

/* -------------------------------------------------------------------------
header files and macros
-------------------------------------------------------------------------- */

// #define	_GNU_SOURCE			/* for strcasestr() in string.h */
// char	*strcasestr();			/* non-standard extension */
// #define _GNU_SOURCE char *strcasestr(const char *haystack, const char *needle);
const char *strcasestr(const char *s1, const char *s2);

/* --- standard headers --- */
#include <stdio.h>
#include <stdlib.h>
/*#include <unistd.h>*/
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
extern	char **environ;		/* for \environment directive */

/* -------------------------------------------------------------------------
messages (used mostly by main() and also by rastmessage())
-------------------------------------------------------------------------- */
static	char *copyright1 =		/* copyright, gnu/gpl notice */
 "+-----------------------------------------------------------------------+\n"
 "|mimeTeX vers " VERSION ", " COPYRIGHTTEXT                             "|\n"
 "+-----------------------------------------------------------------------+\n"
 "| mimeTeX is free software, licensed to you under terms of the GNU/GPL, |\n"
 "|           and comes with absolutely no warranty whatsoever.           |",
*copyright2 =
 "|          See http://www.forkosh.com/mimetex.html for details.         |\n"
 "+-----------------------------------------------------------------------+";
static	int maxmsgnum = 3,		/* maximum msgtable[] index */
	/* --- keep these message numbers updated if table changes --- */
	invmsgnum = 0,			/* general invalid message */
	refmsgnum = 3;			/* urlncmp() failed to validate */
static	char *msgtable[] = {		/* messages referenced by [index] */
 "\\red\\small\\rm\\fbox{\\array{"	/* [0] is invalid_referer_msg */
   "Please~read~www.forkosh.com/mimetex.html\\\\and~install~mimetex.cgi~"
   "on~your~own~server.\\\\Thank~you,~John~Forkosh}}",
 "\\red\\small\\rm\\fbox{\\array{"	/* [1] */
   "Please~provide~your~{\\tiny~HTTP-REFERER}~to~access~the~public\\\\"
   "mimetex~server.~~Or~please~read~~www.forkosh.com/mimetex.html\\\\"
   "and~install~mimetex.cgi~on~your~own~server.~~Thank~you,~John~Forkosh}}",
 "\\red\\small\\rm\\fbox{\\array{"	/* [2] */
   "The~public~mimetex~server~is~for~testing.~~For~production,\\\\"
   "please~read~~www.forkosh.com/mimetex.html~~and~install\\\\"
   "mimetex.cgi~on~your~own~server.~~Thank~you,~John~Forkosh}}",
 "\\red\\small\\rm\\fbox{\\array{"	/* [3] */
   "Only~SERVER_NAME~may~use~mimetex~on~this~server.\\\\"
   "Please~read~~www.forkosh.com/mimetex.html~~and~install\\\\"
   "mimetex.cgi~on~your~own~server.~~Thank~you,~John~Forkosh}}",
 NULL } ;				/* trailer */

/* -------------------------------------------------------------------------
additional symbols
-------------------------------------------------------------------------- */
/* ---
 * windows-specific header info
 * ---------------------------- */
#if !defined(WINDOWS)		/* -DWINDOWS not supplied by user */
  #if defined(_WINDOWS) || defined(_WIN32) || defined(WIN32) \
  ||  defined(DJGPP)		/* try to recognize windows compilers */ \
  ||  defined(_USRDLL)		/* must be WINDOWS if compiling for DLL */
    #define WINDOWS		/* signal windows */
  #endif
#endif
#if defined(WINDOWS)		/* Windows opens stdout in char mode, and */
  #include <fcntl.h>		/* precedes every 0x0A with spurious 0x0D.*/
  #include <io.h>		/* So emitcache() issues a Win _setmode() */
				/* call to put stdout in binary mode. */
  #if defined(_O_BINARY) && !defined(O_BINARY)  /* only have _O_BINARY */
    #define O_BINARY _O_BINARY	/* make O_BINARY available, etc... */
    #define setmode  _setmode
    #define fileno   _fileno
  #endif
  #if defined(_O_BINARY) || defined(O_BINARY)  /* setmode() now available */
    #define HAVE_SETMODE	/* so we'll use setmode() */
  #endif
  #if defined(_MSC_VER)
    #define popen _popen
    #define pclose _pclose
    #define strncasecmp _strnicmp

    #if defined(_DEBUG) /* MS VC++ in debug mode */
    /* to show source file and line numbers where memory leaks occur... */
      #define _CRTDBG_MAP_ALLOC	/* ...include this debug macro */
      #include <crtdbg.h>		/* and this debug library */
    #endif
  #endif
  #define ISWINDOWS 1
#else
  #define ISWINDOWS 0
#endif

/* ---
 * check for supersampling or low-pass anti-aliasing
 * ------------------------------------------------- */
#if defined(SS)
  #define ISSUPERSAMPLING 1
  #if !defined(AAALGORITHM)
    #define AAALGORITHM 1		/* default supersampling algorithm */
  #endif
  #if !defined(AA)			/* anti-aliasing not explicitly set */
    #define AA				/* so define it ourselves */
  #endif
  #if !defined(SSFONTS)			/* need supersampling fonts */
    #define SSFONTS
  #endif
#else
  #define ISSUPERSAMPLING 0
  #if !defined(AAALGORITHM)
    #define AAALGORITHM 3 /*2*/		/* default lowpass algorithm */
  #endif
#endif
#if !defined(MAXFOLLOW)
  #define MAXFOLLOW 8			/* aafollowline() maxturn default */
#endif

/* ---
 * set aa (and default gif) if any anti-aliasing options specified
 * --------------------------------------------------------------- */
#if defined(AA) || defined(GIF) || defined(PNG) \
||  defined(CENTERWT) || defined(ADJACENTWT) || defined(CORNERWT) \
||  defined(MINADJACENT) || defined(MAXADJACENT)
  #if !defined(GIF) && !defined(AA)	/* aa not explicitly specified */
    #define AA				/* so define it ourselves */
  #endif
  #if !defined(GIF) && !defined(PNG)	/* neither gif nor png specified */
    #define GIF				/* so default to gif */
  #endif
#endif
/* --- resolve output option inconsistencies --- */
#if defined(XBITMAP)			/* xbitmap supercedes gif and png */
  #if defined(AA)
    #undef AA
  #endif
  #if defined(GIF)
    #undef GIF
  #endif
  #if defined(PNG)
    #undef PNG
  #endif
#endif

/* ---
 * decide whether or not to compile main()
 * --------------------------------------- */
#if defined(XBITMAP) || defined(GIF) || defined(PNG)
  /* --- yes, compile main() --- */
  #define DRIVER			/* main() driver will be compiled */
#else /* --- main() won't be compiled (e.g., for gfuntype.c) --- */
  #if !defined(TEXFONTS)
    #define NOTEXFONTS			/* texfonts not required */
  #endif
#endif

/* ---
 * application headers
 * ------------------- */
#if !defined(NOTEXFONTS) && !defined(TEXFONTS)
  #define TEXFONTS			/* to include texfonts.h */
#endif
#include "mimetex.h"

/* ---
 * info needed when gif image returned in memory buffer
 * ---------------------------------------------------- */
#if defined(GIF)			/* compiling along with gifsave.c */
  extern int gifSize;
  extern int maxgifSize;
#else					/* or just set dummy values */
  static int gifSize=0, maxgifSize=0;
#endif
/* --- gamma correction --- */
#if !defined(GAMMA)
  #define GAMMA 1.25 /*1.75*/ /*2.2*/
#endif
#if !defined(REVERSEGAMMA)
  #define REVERSEGAMMA 0.5		/* for \reverse white-on-black */
#endif
/* --- opaque background (default to transparent) --- */
#if !defined(OPAQUE)
  #define ISTRANSPARENT 1
#else
  #define ISTRANSPARENT 0
#endif

/* ---
 * internal buffer sizes
 * --------------------- */
#if !defined(MAXEXPRSZ)
  #define MAXEXPRSZ (32768-1)		/*max #bytes in input tex expression*/
#endif
#if !defined(MAXSUBXSZ)
  #define MAXSUBXSZ (((MAXEXPRSZ+1)/2)-1)/*max #bytes in input subexpression*/
#endif
#if !defined(MAXTOKNSZ)
  #define MAXTOKNSZ (((MAXSUBXSZ+1)/4)-1) /* max #bytes in input token */
#endif
#if !defined(MAXFILESZ)
  #define MAXFILESZ (65536-1)		/*max #bytes in input (output) file*/
#endif
#if !defined(MAXLINESZ)
  #define MAXLINESZ (4096-1)		/* max #chars in line from file */
#endif
#if !defined(MAXGIFSZ)
  #define MAXGIFSZ 131072		/* max #bytes in output GIF image */
#endif

/* ---
 * note: point3d.x,y,z are relative to the origin 0.,0.,0. of an abstract
 * coordinate system, calculated to be at the center pixel of a raster.
 * point3d and matrix3d are used for rotations, as per
 *   https://en.wikipedia.org/wiki/Rotation_matrix#In_three_dimensions
 * in function rotrast3d()
 * ------------------------------------------------------------------------ */
/* --- 3d-point --- */
#define point3d struct point3d_struct   /* "typedef" for point3d_struct */
point3d {
  /* --- x,y,z-coords relative to 0.,0.,0. origin of abstract coord axes --- */
  double x;                             /* x-coord */
  double y;                             /* y-coord */
  double z;                             /* z-coord */
  } ; /* --- end-of-point3d_struct --- */
/* --- 3d-matrix (rotation matrix) stored row-wise --- */
#define matrix3d struct matrix3d_struct /* "typedef" for matrix3d_struct */
matrix3d {
  /* --- 3x3 matrix stored row-wise --- */
  point3d xrow;                         /* x-row */
  point3d yrow;                         /* y-row */
  point3d zrow;                         /* z-row */
  } ; /* --- end-of-matrix3d_struct --- */
/* --- 3d-rotation parameters --- */
#define	YAXIS {0.,1.,0.}		/* x-component=0, y=1, z=0 */
#define	UTHETA 0.0			/* no rotation (in degrees) */
GLOBAL(point3d,uaxis,YAXIS);		/* rotate around y-axis */
GLOBAL(double,utheta,UTHETA);		/* rotation amount in degrees */

/* -------------------------------------------------------------------------
adjustable default values
-------------------------------------------------------------------------- */
/* ---
 * anti-aliasing parameters
 * ------------------------ */
#if !defined(CENTERWT)
  /*#define CENTERWT 32*/		/* anti-aliasing centerwt default */
  /*#define CENTERWT 10*/		/* anti-aliasing centerwt default */
  #define CENTERWT 8			/* anti-aliasing centerwt default */
#endif
#if !defined(ADJACENTWT)
  /*#define ADJACENTWT 3*/		/* anti-aliasing adjacentwt default*/
  #define ADJACENTWT 2			/* anti-aliasing adjacentwt default*/
#endif
#if !defined(CORNERWT)
  #define CORNERWT 1			/* anti-aliasing cornerwt default*/
#endif
#if !defined(MINADJACENT)
  #define MINADJACENT 6			/*anti-aliasing minadjacent default*/
#endif
#if !defined(MAXADJACENT)
  #define MAXADJACENT 8			/*anti-aliasing maxadjacent default*/
#endif
/* --- variables for anti-aliasing parameters --- */
GLOBAL(int,centerwt,CENTERWT);		/*lowpass matrix center pixel wt */
GLOBAL(int,adjacentwt,ADJACENTWT);	/*lowpass matrix adjacent pixel wt*/
GLOBAL(int,cornerwt,CORNERWT);		/*lowpass matrix corner pixel wt */
GLOBAL(int,minadjacent,MINADJACENT);	/* darken if>=adjacent pts black*/
GLOBAL(int,maxadjacent,MAXADJACENT);	/* darken if<=adjacent pts black */
GLOBAL(int,weightnum,1);		/* font wt, */
GLOBAL(int,maxaaparams,4);		/* #entries in table */
/* --- anti-aliasing parameter values by font weight --- */
#define	aaparameters struct aaparameters_struct /* typedef */
aaparameters
  { int	centerwt;			/* lowpass matrix center   pixel wt*/
    int	adjacentwt;			/* lowpass matrix adjacent pixel wt*/
    int cornerwt;			/* lowpass matrix corner   pixel wt*/
    int	minadjacent;			/* darken if >= adjacent pts black */
    int	maxadjacent;			/* darken if <= adjacent pts black */
    int fgalias,fgonly,bgalias,bgonly; } ; /* aapnm() params */
STATIC aaparameters aaparams[]		/* set params by weight */
  #ifdef INITVALS
  =
  { /* ----------------------------------------------------
    centerwt adj corner minadj max  fgalias,only,bgalias,only
    ------------------------------------------------------- */
	{ 64,  1,  1,    6,  8,     1,0,0,0 },	/* 0 = light */
	{ CENTERWT,ADJACENTWT,CORNERWT,MINADJACENT,MAXADJACENT,1,0,0,0 },
	{ 8,   1,  1,    5,  8,     1,0,0,0 },	/* 2 = semibold */
	{ 8,   2,  1,    4,  9,     1,0,0,0 }	/* 3 = bold */
  } /* --- end-of-aaparams[] --- */
  #endif
  ;
/* --- anti-aliasing diagnostics (to help improve algorithm) --- */
STATIC int patternnumcount0[99], patternnumcount1[99], /*aalookup() counts*/
	ispatternnumcount = 1;		/* true to accumulate counts */

/* -------------------------------------------------------------------------
other variables
-------------------------------------------------------------------------- */
/* --- black on white background (default), or white on black --- */
#if defined(WHITE)
  #define ISBLACKONWHITE 0		/* white on black background */
#else
  #define ISBLACKONWHITE 1		/* black on white background */
#endif
/* --- colors --- */
#define	BGRED   (ISBLACKONWHITE?255:0)
#define	BGGREEN (ISBLACKONWHITE?255:0)
#define	BGBLUE  (ISBLACKONWHITE?255:0)
#if !defined(FGRED)
  #define FGRED   (ISBLACKONWHITE?0:255)
#endif
#if !defined(FGGREEN)
  #define FGGREEN (ISBLACKONWHITE?0:255)
#endif
#if !defined(FGBLUE)
  #define FGBLUE  (ISBLACKONWHITE?0:255)
#endif
/* --- advertisement
   one image in every ADFREQUENCY is wrapped in "advertisement" --- */
#if !defined(ADFREQUENCY)
  #define ADFREQUENCY 0			/* never show advertisement if 0 */
#endif
#ifndef	HOST_SHOWAD
  #define HOST_SHOWAD "\000"		/* show ads on all hosts */
#endif
/* --- mathTeX info for \mathtex{} directive --- */
#if !defined(MATHTEX)			/* localhost:// or http://url/ */
  #define MATHTEX "localhost://mathtex.cgi"
#endif
#if !defined(MATHTEXPWD)		/* mathTeX \password{} */
  #define MATHTEXPWD "\000"
#endif
#if !defined(WGET)			/* local /path/to/wget */
  #define WGET "wget"
#endif
/* --- "smash" margin (0 means no smashing) --- */
#if !defined(SMASHMARGIN)
  #if defined(NOSMASH)
    #define SMASHMARGIN 0
  #else
    #define SMASHMARGIN 3
  #endif
#endif
#if !defined(SMASHCHECK)
  #define SMASHCHECK 0
#endif
/* --- textwidth --- */
#if !defined(TEXTWIDTH)
  #define TEXTWIDTH (400)
#endif
/* --- font "combinations" --- */
#define	CMSYEX (109)			/*select CMSY10, CMEX10 or STMARY10*/
/* --- prefix prepended to all expressions --- */
#if !defined(PREFIX)
  #define PREFIX "\000"			/* default no prepended prefix */
#endif
/* --- skip argv[]'s preceding ARGSIGNAL when parsing command-line args --- */
#if defined(NOARGSIGNAL)
  #define ARGSIGNAL NULL
#endif
#if !defined(ARGSIGNAL)
  #define ARGSIGNAL "++"
#endif
/* --- security and logging (inhibit message logging, etc) --- */
#if !defined(SECURITY)
  #define SECURITY 999			/* default highest security level */
#endif
#if !defined(LOGFILE)
  #define LOGFILE "mimetex.log"		/* default log file */
#endif
#if !defined(CACHELOG)
  #define CACHELOG "mimetex.log"	/* default caching log file */
#endif
#if !defined(NODUMPENVP) && !defined(DUMPENVP)
  #define DUMPENVP			/* assume char *envp[] available */
#endif
/* --- max query_string length if no http_referer supplied --- */
#if !defined(NOREFMAXLEN)
  #define NOREFMAXLEN 9999		/* default to any length query */
#endif
#if !defined(NOREFSAFELEN)
  #define NOREFSAFELEN 24		/* too small for hack exploit */
#endif
/* --- check whether or not to perform http_referer check --- */
#if defined(REFERER)			/* only specified referers allowed */
  #undef NOREFMAXLEN
  #define NOREFMAXLEN NOREFSAFELEN
#else					/* all http_referer's allowed */
  #define REFERER NULL
#endif
/* --- check top levels of http_referer against server_name --- */
#if defined(REFLEVELS)			/* #topmost levels to check */
  #undef NOREFMAXLEN
  #define NOREFMAXLEN NOREFSAFELEN
#else
  #if defined(NOREFCHECK)
    #define REFLEVELS 0			/* don't match host and referer */
  #else
    #define REFLEVELS 3			/* default matches abc.def.com */
  #endif
#endif
/* --- password to bypass preceding REFERER checks --- */
#if !defined(PASSWORD)
  #define PASSWORD "\000"               /* \password{PASSWORD} in expression */
#endif
static  char password[128] = "\000";    /* user's \password{password} */
static  int  ispassword = 0;            /* true if password==PASSWORD */
/* --- check whether or not \input, \counter, \environment permitted --- */
#if defined(DEFAULTSECURITY)		/* default security specified */
  #define EXPLICITDEFSECURITY		/* don't override explicit default */
#else					/* defualt security not specified */
  #define DEFAULTSECURITY (8)		/* so set default security level */
#endif
#if defined(INPUTREFERER)		/*http_referer's permitted to \input*/
  #if !defined(INPUTSECURITY)		/* so we need to permit \input{} */
    #define INPUTSECURITY (99999)	/* make sure SECURITY<INPUTSECURITY */
  #endif
#else					/* no INPUTREFERER list supplied */
  #define INPUTREFERER NULL		/* so init it as NULL pointer */
#endif
#if !defined(INPUTPATH)			/* \input{} paths permitted for... */
  #define INPUTPATH NULL		/* ...any referer */
#endif
#if !defined(INPUTSECURITY)		/* \input{} security not specified */
  #if defined(INPUTOK)			/* but INPUTOK flag specified */
    #define INPUTSECURITY (99999)	/* so enable \input{} */
    #if !defined(EXPLICITDEFSECURITY)	/* don't override explicit default */
      #undef  DEFAULTSECURITY		/* but we'll override our default */
      #define DEFAULTSECURITY (99999)	/*let -DINPUTOK enable \counter,etc*/
    #endif
  #else					/* else no \input{} specified */
    #define INPUTSECURITY DEFAULTSECURITY /* set default \input security */
  #endif
#endif
#if !defined(COUNTERSECURITY)		/*\counter{} security not specified*/
  #if defined(COUNTEROK)		/* but COUNTEROK flag specified */
    #define COUNTERSECURITY (99999)	/* so enable \counter{} */
  #else					/* else no \counter{} specified */
    #define COUNTERSECURITY DEFAULTSECURITY /*set default \counter security*/
  #endif
#endif
#if !defined(ENVIRONSECURITY)		/* \environ security not specified */
  #if defined(ENVIRONOK)		/* but ENVIRONOK flag specified */
    #define ENVIRONSECURITY (99999)	/* so enable \environ */
  #else					/* else no \environ specified */
    #define ENVIRONSECURITY DEFAULTSECURITY /*set default \environ security*/
  #endif
#endif
/* --- image caching (cache images if given -DCACHEPATH=\"path\") --- */
#if !defined(CACHEPATH)
  #define ISCACHING 0			/* no caching */
  #define CACHEPATH "\000"		/* same directory as mimetex.cgi */
#else
  #define ISCACHING 1			/* caching if -DCACHEPATH="path" */
#endif
/* --- \input paths (prepend prefix if given -DPATHPREFIX=\"prefix\") --- */
#if !defined(PATHPREFIX)
  #define PATHPREFIX "\000"		/* paths relative mimetex.cgi */
#endif
/* --- time zone delta t (in hours) --- */
#if !defined(TZDELTA)
  #define TZDELTA 0
#endif
/* --- treat +'s in query string as blanks? --- */
#if defined(PLUSBLANK)			/* + always interpreted as blank */
  #define ISPLUSBLANK 1
#else
  #if defined(PLUSNOTBLANK)		/* + never interpreted as blank */
    #define ISPLUSBLANK 0
  #else					/* program tries to determine */
    #define ISPLUSBLANK (-1)
  #endif
#endif

/* -------------------------------------------------------------------------
debugging and logging / error reporting
-------------------------------------------------------------------------- */
/* --- debugging and error reporting --- */
#if !defined(MSGLEVEL)
  #define MSGLEVEL 1
#endif
#define	DBGLEVEL 9			/* debugging if msglevel>=DBGLEVEL */
#define	LOGLEVEL 3			/* logging if msglevel>=LOGLEVEL */
#if !defined(FORMLEVEL)
  #define FORMLEVEL LOGLEVEL		/*msglevel if called from html form*/
#endif
#if !defined(ERRORSTATUS)		/* exit(ERRORSTATUS) for any error */
  #define ERRORSTATUS 0			/* default doesn't signal errors */
#endif
GLOBAL(int,seclevel,SECURITY);		/* security level */
GLOBAL(int,inputseclevel,INPUTSECURITY); /* \input{} security level */
GLOBAL(int,counterseclevel,COUNTERSECURITY); /* \counter{} security level */
GLOBAL(int,environseclevel,ENVIRONSECURITY); /* \environ{} security level */
GLOBAL(int,msglevel,MSGLEVEL);		/* message level for verbose/debug */
GLOBAL(int,errorstatus,ERRORSTATUS);	/* exit status if error encountered*/
GLOBAL(int,exitstatus,0);		/* exit status (0=success) */
STATIC	FILE *msgfp;			/* output in command-line mode */
/* --- logging macros --- */
#define	logdump(lvl,msg)  if(msglevel>=(lvl)) {logmsg(msg);}else
#define	logmsg(msg) if(1) {logger(msgfp,msglevel,(msg),NULL);}else
/* --- embed warnings in rendered expressions, [\xxx?] if \xxx unknown --- */
#if defined(WARNINGS)
  #define WARNINGLEVEL WARNINGS
#else
  #if defined(NOWARNINGS)
    #define WARNINGLEVEL 0
  #else
    #define WARNINGLEVEL 1
  #endif
#endif
GLOBAL(int,warninglevel,WARNINGLEVEL);	/* warning level */

/* -------------------------------------------------------------------------
control flags and values
-------------------------------------------------------------------------- */
GLOBAL(int,isquery,0);			/* true=cgi?query; false=commandline*/
GLOBAL(int,daemonlevel,0);		/* incremented in main() */
GLOBAL(int,recurlevel,0);		/* inc/decremented in rasterize() */
GLOBAL(int,scriptlevel,0);		/* inc/decremented in rastlimits() */
GLOBAL(int,isstring,0);			/*pixmap is ascii string, not raster*/
GLOBAL(int,isligature,0);		/* true if ligature found */
GLOBAL(char,*subexprptr,(char *)NULL);	/* ptr within expression to subexpr*/
/*SHARED(int,imageformat,1);*/		/* image is 1=bitmap, 2=.gf-like */
GLOBAL(int,isdisplaystyle,1);		/* displaystyle mode (forced if 2) */
GLOBAL(int,ispreambledollars,0);	/* displaystyle mode set by $$...$$ */
GLOBAL(int,ninputcmds,0);		/* # of \input commands processed */
GLOBAL(int,fontnum,0);			/* cal=1,scr=2,rm=3,it=4,bb=5,bf=6 */
GLOBAL(int,fontsize,NORMALSIZE);	/* current size */
GLOBAL(int,magstep,1);			/* magstep (1=no change) */
GLOBAL(int,displaysize,DISPLAYSIZE);	/* use \displaystyle when fontsize>=*/
GLOBAL(int,shrinkfactor,3);		/* shrinkfactors[fontsize] */
GLOBAL(int,rastlift,0);			/* rastraise() lift parameter */
GLOBAL(int,rastlift1,0);		/* rastraise() lift for base exprssn*/
GLOBAL(double,unitlength,1.0);		/* #pixels per unit (may be <1.0) */
GLOBAL(int,iunitlength,1);		/* #pixels per unit as int for store*/
/*GLOBAL(int,textwidth,TEXTWIDTH);*/	/* #pixels across line */
GLOBAL(int,adfrequency,ADFREQUENCY);	/* advertisement frequency */
GLOBAL(int,isnocatspace,0);		/* >0 to not add space in rastcat()*/
GLOBAL(int,smashmargin,SMASHMARGIN);	/* minimum "smash" margin */
GLOBAL(int,mathsmashmargin,SMASHMARGIN); /* needed for \text{if $n-m$ even}*/
GLOBAL(int,issmashdelta,1);		/* true if smashmargin is a delta */
GLOBAL(int,isexplicitsmash,0);		/* true if \smash explicitly given */
GLOBAL(int,smashcheck,SMASHCHECK);	/* check if terms safe to smash */
GLOBAL(int,isnomath,0);			/* true to inhibit math mode */
GLOBAL(int,isscripted,0);		/* is (lefthand) term text-scripted*/
GLOBAL(int,isdelimscript,0);		/* is \right delim text-scripted */
GLOBAL(int,issmashokay,0);		/*is leading char okay for smashing*/
#define	BLANKSIGNAL (-991234)		/*rastsmash signal right-hand blank*/
GLOBAL(int,blanksignal,BLANKSIGNAL);	/*rastsmash signal right-hand blank*/
GLOBAL(int,blanksymspace,0);		/* extra (or too much) space wanted*/
GLOBAL(int,istransparent,ISTRANSPARENT);/* true sets background transparent*/
GLOBAL(int,fgred,FGRED);
  GLOBAL(int,fggreen,FGGREEN);
  GLOBAL(int,fgblue,FGBLUE);		/* fg r,g,b */
GLOBAL(int,bgred,BGRED);
  GLOBAL(int,bggreen,BGGREEN);
  GLOBAL(int,bgblue,BGBLUE);		/* bg r,g,b */
GLOBAL(double,gammacorrection,GAMMA);	/* gamma correction */
GLOBAL(int,isplusblank,ISPLUSBLANK);	/*interpret +'s in query as blanks?*/
GLOBAL(int,isblackonwhite,ISBLACKONWHITE); /*1=black on white,0=reverse*/
GLOBAL(char,exprprefix[256],PREFIX);	/* prefix prepended to expressions */
GLOBAL(int,aaalgorithm,AAALGORITHM);	/* for lp, 1=aalowpass, 2 =aapnm */
GLOBAL(int,maxfollow,MAXFOLLOW);	/* aafollowline() maxturn parameter*/
GLOBAL(int,fgalias,1);
  GLOBAL(int,fgonly,0);
  GLOBAL(int,bgalias,0);
  GLOBAL(int,bgonly,0);			/* aapnm() params */
GLOBAL(int,issupersampling,ISSUPERSAMPLING); /*1=supersampling 0=lowpass*/
GLOBAL(int,isss,ISSUPERSAMPLING);	/* supersampling flag for main() */
GLOBAL(int,ispbmpgm,0);			/* true for pbm/pgm instead of gif */
GLOBAL(int,pbmpgmtype,0);		/* 1=pbm / 2=pgm (else error) */
GLOBAL(int,*workingparam,(int *)NULL);	/* working parameter */
GLOBAL(subraster,*workingbox,(subraster *)NULL); /*working subraster box*/
GLOBAL(int,isreplaceleft,0);		/* true to replace leftexpression */
GLOBAL(subraster,*leftexpression,(subraster *)NULL); /*rasterized so far*/
GLOBAL(mathchardef,*leftsymdef,NULL);	/* mathchardef for preceding symbol*/
GLOBAL(int,fraccenterline,NOVALUE);	/* baseline for punct. after \frac */
/*GLOBAL(int,currentcharclass,NOVALUE);*/ /*primarily to check for PUNCTION*/
GLOBAL(int,iscaching,ISCACHING);	/* true if caching images */
GLOBAL(char,cachepath[256],CACHEPATH);	/* relative path to cached files */
GLOBAL(int,isemitcontenttype,1);	/* true to emit mime content-type */
int	iscachecontenttype = 0;		/* true to cache mime content-type */
char	contenttype[2048] = "\000";	/* content-type:, etc buffer */
GLOBAL(char,pathprefix[256],PATHPREFIX); /*prefix for \input,\counter paths*/
/*GLOBAL(int,iswindows,ISWINDOWS);*/	/* true if compiled for ms windows */

/* -------------------------------------------------------------------------
store for evalterm() [n.b., these are stripped-down funcs from nutshell]
-------------------------------------------------------------------------- */
#define	STORE struct store_struct	/* "typedef" for store struct */
#define	MAXSTORE 100			/* max 100 identifiers */
STORE {
  char	*identifier;			/* identifier */
  int	*value;				/* address of corresponding value */
  } ; /* --- end-of-store_struct --- */
static STORE mimestore[MAXSTORE] = {
    { "fontsize", &fontsize },	{ "fs", &fontsize },	/* font size */
    { "fontnum", &fontnum },	{ "fn", &fontnum },	/* font number */
    { "unitlength", &iunitlength },			/* unitlength */
    /*{ "mytestvar", &mytestvar },*/
    { NULL, NULL }					/* end-of-store */
  } ; /* --- end-of-mimestore[] --- */

/* -------------------------------------------------------------------------
other application global data
-------------------------------------------------------------------------- */
/* --- getdirective() global data --- */
static	int  argformat   = 0;		/* 111... if arg not {}-enclosed */
static	int  optionalpos = 0;		/* # {args} before optional [args] */
static	int  noptional   = 0;		/* # optional [args] found */
static	char optionalargs[10][128] =	/* buffer for optional args */
  { "\000", "\000", "\000", "\000", "\000", "\000", "\000", "\000" };
/* --- **dump/debug** data --- */
int	isdumpgif = 0;			/* **debug** dump gif image */
#if !defined(DUMPGIF)
  #define DUMPGIF "dumpgif"		/* use .gif_dumpgif extension */
#endif

/* -------------------------------------------------------------------------
miscellaneous macros
-------------------------------------------------------------------------- */
#if 0	/* --- these are now #define'd in mimetex.h --- */
#define	max2(x,y)  ((x)>(y)? (x):(y))	/* larger of 2 arguments */
#define	min2(x,y)  ((x)<(y)? (x):(y))	/* smaller of 2 arguments */
#define	max3(x,y,z) max2(max2(x,y),(z))	/* largest of 3 arguments */
#define	min3(x,y,z) min2(min2(x,y),(z))	/* smallest of 3 arguments */
#define absval(x)  ((x)>=0?(x):(-(x)))	/* absolute value */
#define	iround(x)  ((int)((x)>=0?(x)+0.5:(x)-0.5)) /* round double to int */
#define	dmod(x,y)  ((x)-((y)*((double)((int)((x)/(y)))))) /*x%y for doubles*/
#endif
#define compress(s,c) if((s)!=NULL)	/* remove embedded c's from s */ \
	{ char *p; while((p=strchr((s),(c)))!=NULL) {strsqueeze(p,1);} } else
#define	slower(s)  if ((s)!=NULL)	/* lowercase all chars in s */ \
	{ char *p=(s); while(*p!='\000'){*p=tolower(*p); p++;} } else
/*subraster *subrastcpy();*/		/* need global module declaration */
/*#define spnosmash(sp) if (sp->type==CHARASTER) sp=subrastcpy(sp); \ */
/*	sp->type=blanksignal */
/* ---evaluate \directive[arg] or \directive{arg} scaled by unitlength--- */
#define	eround(arg) (iround(unitlength*((double)evalterm(mimestore,(arg)))))
/* --- check if a string is empty --- */
#define	isempty(s)  ((s)==NULL?1:(*(s)=='\000'?1:0))
/* --- last char of a string --- */
#define	lastchar(s) (isempty(s)?'\000':*((s)+(strlen(s)-1)))
/* --- lowercase a string --- */
#define	strlower(s) strnlower((s),0)	/* lowercase an entire string */
/* --- strip leading and trailing whitespace (including ~) --- */
#define	trimwhite(thisstr) if ( (thisstr) != NULL ) { \
	int thislen = strlen(thisstr); \
	while ( --thislen >= 0 ) \
	  if ( isthischar((thisstr)[thislen]," \t\n\r\f\v") ) \
	    (thisstr)[thislen] = '\000'; \
	  else break; \
	if ( (thislen = strspn((thisstr)," \t\n\r\f\v")) > 0 ) \
	  {strsqueeze((thisstr),thislen);} } else
/* --- strncpy() n bytes and make sure it's null-terminated --- */
#define	strninit(target,source,n) if( (target)!=NULL && (n)>=0 ) { \
	  char *thissource = (source); \
	  (target)[0] = '\000'; \
	  if ( (n)>0 && thissource!=NULL ) { \
	    strncpy((target),thissource,(n)); \
	    (target)[(n)] = '\000'; } }
/* --- strcpy(s,s+n) using memmove() (also works for negative n) --- */
#define	strsqueeze(s,n) if((n)!=0) { if(!isempty((s))) { \
	int thislen3=strlen(s); \
	if ((n) >= thislen3) *(s) = '\000'; \
	else memmove(s,s+(n),1+thislen3-(n)); }} else/*user supplies final;*/
/* --- strsqueeze(s,t) with two pointers --- */
#define	strsqueezep(s,t) if(!isempty((s))&&!isempty((t))) { \
	int sqlen=strlen((s))-strlen((t)); \
	if (sqlen>0 && sqlen<=999) {strsqueeze((s),sqlen);} } else
/* --- skip/find whitespace (from mathtex.c for getdirective()) --- */
#define	findwhite(thisstr)  while ( !isempty(thisstr) ) \
	if ( isthischar(*(thisstr),WHITESPACE) ) break; else (thisstr)++
	/* thisstr += strcspn(thisstr,WHITESPACE) */
/* --- skip \command (i.e., find char past last char of \command) --- */
#define	skipcommand(thisstr)  while ( !isempty(thisstr) ) \
	if ( !isalpha(*(thisstr)) ) break; else (thisstr)++

/* ---
 * PART2
 * ------ */
#if !defined(PARTS) || defined(PART2)
/* ==========================================================================
 * Function:	new_raster ( width, height, pixsz )
 * Purpose:	Allocation and constructor for raster.
 *		mallocs and initializes memory for width*height pixels,
 *		and returns raster struct ptr to caller.
 * --------------------------------------------------------------------------
 * Arguments:	width (I)	int containing width, in bits,
 *				of raster pixmap to be allocated
 *		height (I)	int containing height, in bits/scans,
 *				of raster pixmap to be allocated
 *		pixsz (I)	int containing #bits per pixel, 1 or 8
 * --------------------------------------------------------------------------
 * Returns:	( raster * )	ptr to allocated and initialized
 *				raster struct, or NULL for any error.
 * --------------------------------------------------------------------------
 * Notes:
 * ======================================================================= */
/* --- entry point --- */
raster	*new_raster ( int width, int height, int pixsz )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
raster	*rp = (raster *)NULL;		/* raster ptr returned to caller */
pixbyte	*pixmap = NULL;			/* raster pixel map to be malloced */
int	nbytes = pixsz*bitmapsz(width,height); /* #bytes needed for pixmap */
int	filler = (isstring?' ':0);	/* pixmap filler */
int	delete_raster();		/* in case pixmap malloc() fails */
int	npadding = (0&&issupersampling?8+256:0); /* padding bytes */
/* -------------------------------------------------------------------------
allocate and initialize raster struct and embedded bitmap
-------------------------------------------------------------------------- */
if ( msgfp!=NULL && msglevel>=9999 )
  { fprintf(msgfp,"new_raster(%d,%d,%d)> entry point\n",
    width,height,pixsz); fflush(msgfp); }
/* --- allocate and initialize raster struct --- */
rp = (raster *)malloc(sizeof(raster));	/* malloc raster struct */
if ( msgfp!=NULL && msglevel>=9999 )
  { fprintf(msgfp,"new_raster> rp=malloc(%d) returned (%s)\n",
    (int)sizeof(raster),(rp==NULL?"null ptr":"success")); fflush(msgfp); }
if ( rp == (raster *)NULL )		/* malloc failed */
  goto end_of_job;			/* return error to caller */
rp->width = width;			/* store width in raster struct */
rp->height = height;			/* and store height */
rp->format = 1;				/* initialize as bitmap format */
rp->pixsz = pixsz;			/* store #bits per pixel */
rp->pixmap = (pixbyte *)NULL;		/* init bitmap as null ptr */
/* --- allocate and initialize bitmap array --- */
if ( msgfp!=NULL && msglevel>=9999 )
  { fprintf(msgfp,"new_raster> calling pixmap=malloc(%d)\n",
    nbytes); fflush(msgfp); }
if ( nbytes>0 && nbytes<=pixsz*maxraster )  /* fail if width*height too big*/
  pixmap = (pixbyte *)malloc(nbytes+npadding); /*bytes for width*height bits*/
if ( msgfp!=NULL && msglevel>=9999 )
  { fprintf(msgfp,"new_raster> pixmap=malloc(%d) returned (%s)\n",
    nbytes,(pixmap==NULL?"null ptr":"success")); fflush(msgfp); }
if ( pixmap == (pixbyte *)NULL )	/* malloc failed */
  { delete_raster(rp);			/* so free everything */
    rp = (raster *)NULL;		/* reset pointer */
    goto end_of_job; }			/* and return error to caller */
memset((void *)pixmap,filler,nbytes);	/* init bytes to binary 0's or ' 's*/
*pixmap = (pixbyte)0;			/* and first byte always 0 */
rp->pixmap = pixmap;			/* store ptr to malloced memory */
/* -------------------------------------------------------------------------
Back to caller with address of raster struct, or NULL ptr for any error.
-------------------------------------------------------------------------- */
end_of_job:
  if ( msgfp!=NULL && msglevel>=9999 )
    { fprintf(msgfp,"new_raster(%d,%d,%d)> returning (%s)\n",
      width,height,pixsz,(rp==NULL?"null ptr":"success")); fflush(msgfp); }
  return ( rp );			/* back to caller with raster */
} /* --- end-of-function new_raster() --- */


/* ==========================================================================
 * Function:	new_subraster ( width, height, pixsz )
 * Purpose:	Allocate a new subraster along with
 *		an embedded raster of width x height.
 * --------------------------------------------------------------------------
 * Arguments:	width (I)	int containing width of embedded raster
 *		height (I)	int containing height of embedded raster
 *		pixsz (I)	int containing #bits per pixel, 1 or 8
 * --------------------------------------------------------------------------
 * Returns:	( subraster * )	ptr to newly-allocated subraster,
 *				or NULL for any error.
 * --------------------------------------------------------------------------
 * Notes:     o	if width or height <=0, embedded raster not allocated
 * ======================================================================= */
/* --- entry point --- */
subraster *new_subraster ( int width, int height, int pixsz )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
subraster *sp=NULL;			/* subraster returned to caller */
raster	*new_raster(), *rp=NULL;	/* image raster embedded in sp */
int	delete_subraster();		/* in case new_raster() fails */
int	size = NORMALSIZE,		/* default size */
	baseline = height-1;		/* and baseline */
/* -------------------------------------------------------------------------
allocate and initialize subraster struct
-------------------------------------------------------------------------- */
if ( msgfp!=NULL && msglevel>=9999 )
  { fprintf(msgfp,"new_subraster(%d,%d,%d)> entry point\n",
    width,height,pixsz); fflush(msgfp); }
/* --- allocate subraster struct --- */
sp = (subraster *)malloc(sizeof(subraster));  /* malloc subraster struct */
if ( sp == (subraster *)NULL )		/* malloc failed */
  goto end_of_job;			/* return error to caller */
/* --- initialize subraster struct --- */
sp->type = NOVALUE;			/* character or image raster */
sp->symdef =  (mathchardef *)NULL;	/* mathchardef identifying image */
sp->baseline = baseline;		/*0 if image is entirely descending*/
sp->size = size;			/* font size 0-4 */
sp->toprow = sp->leftcol = (-1);	/* upper-left corner of subraster */
sp->image = (raster *)NULL;		/*ptr to bitmap image of subraster*/
/* -------------------------------------------------------------------------
allocate raster and embed it in subraster, and return to caller
-------------------------------------------------------------------------- */
/* --- allocate raster struct if desired --- */
if ( width>0 && height>0 && pixsz>0 )	/* caller wants raster */
  { if ( (rp=new_raster(width,height,pixsz)) /* allocate embedded raster */
    !=   NULL )				/* if allocate succeeded */
        sp->image = rp;			/* embed raster in subraster */
    else				/* or if allocate failed */
      { delete_subraster(sp);		/* free non-unneeded subraster */
	sp = NULL; } }			/* signal error */
/* --- back to caller with new subraster or NULL --- */
end_of_job:
  if ( msgfp!=NULL && msglevel>=9999 )
    { fprintf(msgfp,"new_subraster(%d,%d,%d)> returning (%s)\n",
      width,height,pixsz,(sp==NULL?"null ptr":"success")); fflush(msgfp); }
  return ( sp );
} /* --- end-of-function new_subraster() --- */


/* ==========================================================================
 * Function:	new_chardef (  )
 * Purpose:	Allocates and initializes a chardef struct,
 *		but _not_ the embedded raster struct.
 * --------------------------------------------------------------------------
 * Arguments:	none
 * --------------------------------------------------------------------------
 * Returns:	( chardef * )	ptr to allocated and initialized
 *				chardef struct, or NULL for any error.
 * --------------------------------------------------------------------------
 * Notes:
 * ======================================================================= */
/* --- entry point --- */
chardef	*new_chardef (  )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
chardef	*cp = (chardef *)NULL;		/* chardef ptr returned to caller */
/* -------------------------------------------------------------------------
allocate and initialize chardef struct
-------------------------------------------------------------------------- */
cp = (chardef *)malloc(sizeof(chardef)); /* malloc chardef struct */
if ( cp == (chardef *)NULL )		/* malloc failed */
  goto end_of_job;			/* return error to caller */
cp->charnum = cp->location = 0;		/* init character description */
cp->toprow = cp->topleftcol = 0;	/* init upper-left corner */
cp->botrow = cp->botleftcol = 0;	/* init lower-left corner */
cp->image.width = cp->image.height = 0;	/* init raster dimensions */
cp->image.format = 0;			/* init raster format */
cp->image.pixsz = 0;			/* and #bits per pixel */
cp->image.pixmap = NULL;		/* init raster pixmap as null */
/* -------------------------------------------------------------------------
Back to caller with address of chardef struct, or NULL ptr for any error.
-------------------------------------------------------------------------- */
end_of_job:
  return ( cp );
} /* --- end-of-function new_chardef() --- */


/* ==========================================================================
 * Function:	delete_raster ( rp )
 * Purpose:	Destructor for raster.
 *		Frees memory for raster bitmap and struct.
 * --------------------------------------------------------------------------
 * Arguments:	rp (I)		ptr to raster struct to be deleted.
 * --------------------------------------------------------------------------
 * Returns:	( int )		1 if completed successfully,
 *				or 0 otherwise (for any error).
 * --------------------------------------------------------------------------
 * Notes:
 * ======================================================================= */
/* --- entry point --- */
int	delete_raster ( raster *rp )
{
/* -------------------------------------------------------------------------
free raster bitmap and struct
-------------------------------------------------------------------------- */
if ( rp != (raster *)NULL )		/* can't free null ptr */
  {
  if ( rp->pixmap != (pixbyte *)NULL )	/* can't free null ptr */
    free((void *)rp->pixmap);		/* free pixmap within raster */
  free((void *)rp);			/* lastly, free raster struct */
  } /* --- end-of-if(rp!=NULL) --- */
return ( 1 );				/* back to caller, 1=okay 0=failed */
} /* --- end-of-function delete_raster() --- */


/* ==========================================================================
 * Function:	delete_subraster ( sp )
 * Purpose:	Deallocates a subraster (and embedded raster)
 * --------------------------------------------------------------------------
 * Arguments:	sp (I)		ptr to subraster struct to be deleted.
 * --------------------------------------------------------------------------
 * Returns:	( int )		1 if completed successfully,
 *				or 0 otherwise (for any error).
 * --------------------------------------------------------------------------
 * Notes:
 * ======================================================================= */
/* --- entry point --- */
int	delete_subraster ( subraster *sp )
{
/* -------------------------------------------------------------------------
free subraster struct
-------------------------------------------------------------------------- */
int	delete_raster();		/* to delete embedded raster */
if ( sp != (subraster *)NULL )		/* can't free null ptr */
  {
  if ( sp->type != CHARASTER )		/* not static character data */
    if ( sp->image != NULL )		/*raster allocated within subraster*/
      delete_raster(sp->image);		/* so free embedded raster */
  free((void *)sp);			/* and free subraster struct itself*/
  } /* --- end-of-if(sp!=NULL) --- */
return ( 1 );				/* back to caller, 1=okay 0=failed */
} /* --- end-of-function delete_subraster() --- */


/* ==========================================================================
 * Function:	delete_chardef ( cp )
 * Purpose:	Deallocates a chardef (and bitmap of embedded raster)
 * --------------------------------------------------------------------------
 * Arguments:	cp (I)		ptr to chardef struct to be deleted.
 * --------------------------------------------------------------------------
 * Returns:	( int )		1 if completed successfully,
 *				or 0 otherwise (for any error).
 * --------------------------------------------------------------------------
 * Notes:
 * ======================================================================= */
/* --- entry point --- */
int	delete_chardef ( chardef *cp )
{
/* -------------------------------------------------------------------------
free chardef struct
-------------------------------------------------------------------------- */
if ( cp != (chardef *)NULL )		/* can't free null ptr */
  {
  if ( cp->image.pixmap != NULL )	/* pixmap allocated within raster */
    free((void *)cp->image.pixmap);	/* so free embedded pixmap */
  free((void *)cp);			/* and free chardef struct itself */
  } /* --- end-of-if(cp!=NULL) --- */
/* -------------------------------------------------------------------------
Back to caller with 1=okay, 0=failed.
-------------------------------------------------------------------------- */
return ( 1 );
} /* --- end-of-function delete_chardef() --- */


/* ==========================================================================
 * Function:	rastcpy ( rp )
 * Purpose:	makes duplicate copy of rp
 * --------------------------------------------------------------------------
 * Arguments:	rp (I)		ptr to raster struct to be copied
 * --------------------------------------------------------------------------
 * Returns:	( raster * )	ptr to new copy rp,
 *				or NULL for any error.
 * --------------------------------------------------------------------------
 * Notes:     o
 * ======================================================================= */
/* --- entry point --- */
raster	*rastcpy ( raster *rp )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
raster	*new_raster(), *newrp=NULL;	/*copied raster returned to caller*/
int	height= (rp==NULL?0:rp->height), /* original and copied height */
	width = (rp==NULL?0:rp->width),	/* original and copied width */
	pixsz = (rp==NULL?0:rp->pixsz),	/* #bits per pixel */
	nbytes= (rp==NULL?0:(pixmapsz(rp))); /* #bytes in rp's pixmap */
/* -------------------------------------------------------------------------
allocate copied raster and fill it
-------------------------------------------------------------------------- */
/* --- allocate copied raster with same width,height, and copy bitmap --- */
if ( rp != NULL )			/* nothing to copy if ptr null */
  if ( (newrp = new_raster(width,height,pixsz)) /*same width,height in copy*/
  !=   NULL )				/* check that allocate succeeded */
    memcpy(newrp->pixmap,rp->pixmap,nbytes); /* fill copied raster pixmap */
return ( newrp );			/* return copied raster to caller */
} /* --- end-of-function rastcpy() --- */


/* ==========================================================================
 * Function:	subrastcpy ( sp )
 * Purpose:	makes duplicate copy of sp
 * --------------------------------------------------------------------------
 * Arguments:	sp (I)		ptr to subraster struct to be copied
 * --------------------------------------------------------------------------
 * Returns:	( subraster * )	ptr to new copy sp,
 *				or NULL for any error.
 * --------------------------------------------------------------------------
 * Notes:     o
 * ======================================================================= */
/* --- entry point --- */
subraster *subrastcpy ( subraster *sp )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
subraster *new_subraster(), *newsp=NULL; /* allocate new subraster */
raster	*rastcpy(), *newrp=NULL;	/* and new raster image within it */
int	delete_subraster();		/* dealloc newsp if rastcpy() fails*/
/* -------------------------------------------------------------------------
make copy, and return it to caller
-------------------------------------------------------------------------- */
if ( sp == NULL ) goto end_of_job;	/* nothing to copy */
/* --- allocate new subraster "envelope" for copy --- */
if ( (newsp=new_subraster(0,0,0))	/* allocate subraster "envelope" */
==   NULL ) goto end_of_job;		/* and quit if we fail to allocate */
/* --- transparently copy original envelope to new one --- */
memcpy((void *)newsp,(void *)sp,sizeof(subraster)); /* copy envelope */
/* --- make a copy of the rasterized image itself, if there is one --- */
if ( sp->image != NULL )		/* there's an image embedded in sp */
  if ( (newrp = rastcpy(sp->image))	/* so copy rasterized image in sp */
  ==   NULL )				/* failed to copy successfully */
    { delete_subraster(newsp);		/* won't need newsp any more */
      newsp = NULL;			/* because we're returning error */
      goto end_of_job; }		/* back to caller with error signal*/
/* --- set new params in new envelope --- */
newsp->image = newrp;			/* new raster image we just copied */
switch ( sp->type )			/* set new raster image type */
  { case STRINGRASTER: case CHARASTER: newsp->type = STRINGRASTER; break;
    case ASCIISTRING:                  newsp->type = ASCIISTRING;  break;
    case FRACRASTER:                   newsp->type = FRACRASTER;   break;
    case BLANKSIGNAL:                  newsp->type = blanksignal;  break;
    case IMAGERASTER:  default:        newsp->type = IMAGERASTER;  break; }
/* --- return copy of sp to caller --- */
end_of_job:
  return ( newsp );			/* copy back to caller */
} /* --- end-of-function subrastcpy() --- */


/* ==========================================================================
 * Function:	rastrot ( rp )
 * Purpose:	rotates rp image 90 degrees right/clockwise
 * --------------------------------------------------------------------------
 * Arguments:	rp (I)		ptr to raster struct to be rotated
 * --------------------------------------------------------------------------
 * Returns:	( raster * )	ptr to new raster rotated relative to rp,
 *				or NULL for any error.
 * --------------------------------------------------------------------------
 * Notes:     o	An underbrace is } rotated 90 degrees clockwise,
 *		a hat is <, etc.
 * ======================================================================= */
/* --- entry point --- */
raster	*rastrot ( raster *rp )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
raster	*new_raster(), *rotated=NULL;	/*rotated raster returned to caller*/
int	height = rp->height, irow,	/* original height, row index */
	width = rp->width, icol,	/* original width, column index */
	pixsz = rp->pixsz;		/* #bits per pixel */
/* -------------------------------------------------------------------------
allocate rotated raster and fill it
-------------------------------------------------------------------------- */
/* --- allocate rotated raster with flipped width<-->height --- */
if ( (rotated = new_raster(height,width,pixsz)) /* flip width,height */
!=   NULL )				/* check that allocation succeeded */
  /* --- fill rotated raster --- */
  for ( irow=0; irow<height; irow++ )	/* for each row of rp */
    for ( icol=0; icol<width; icol++ )	/* and each column of rp */
      {	int value = getpixel(rp,irow,icol);
	/* setpixel(rotated,icol,irow,value); } */
	setpixel(rotated,icol,(height-1-irow),value); }
return ( rotated );			/* return rotated raster to caller */
} /* --- end-of-function rastrot() --- */


/* ==========================================================================
 * Function:	rastrot3d ( rp, axis, theta )
 * Purpose:	rotates rp around 3d-axis by theta, projecting back onto
 *		x,y-plane
 * --------------------------------------------------------------------------
 * Arguments:	rp (I)		ptr to raster struct with image to be rotated
 *		axis (I)	point3d * specifying rotation axis
 *		theta (I)	double specifying rotation in degrees
 * --------------------------------------------------------------------------
 * Returns:	( raster * )	ptr to new raster struct with rotated image,
 *				or NULL for any error.
 * --------------------------------------------------------------------------
 * Notes:     o
 * ======================================================================= */
/* --- entry point --- */
raster	*rastrot3d ( raster *rp, point3d *axis, double theta )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
raster	*new_raster(), *rotp = NULL;	/* rotated raster back to caller */
int	width = rp->width,  icol = 0,	/* raster width,  column index */
 	height= rp->height, irow = 0,	/* raster height, row index */
	pixsz = rp->pixsz;		/* #bits per pixel */
double	midcol = ((double)(width))/2.0,	/* x-origin index coord */
	midrow = ((double)(height))/2.0; /* y-origin index coord */
int	bgpixel = 0;			/* background pixel value */
matrix3d *rotmatrix(), *rotmat=NULL;	/* rotation matrix */
point3d	*matmult();			/* matrix multiplication */
/* -------------------------------------------------------------------------
Initialization
-------------------------------------------------------------------------- */
/* --- check input --- */
if ( rp == NULL ) goto end_of_job;	/* error if no input raster supplied */
/* --- get rotation matrix --- */
if ( (rotmat = rotmatrix(axis,theta))	/*rotate around axis by theta degrees*/
==   NULL ) goto end_of_job;		/*some problem, likely 0-length axis*/
/* --- allocate rotated raster (with same width,height) --- */
if ( (rotp = new_raster(width,height,pixsz)) /* (same width,height) */
==   NULL ) goto end_of_job;		/* quit if allocation failed */
/* -------------------------------------------------------------------------
rotate pixel-by-pixel
-------------------------------------------------------------------------- */
for ( irow=0; irow<height; irow++ ) {
  double y = ((double)irow) - midrow;	/* y-coord relative to grid center */
  for ( icol=0; icol<width; icol++ ) {
    int pixval = getpixel(rp,irow,icol); /* pixel value */
    double x = ((double)icol) - midcol; /* x-coord relative to grid center */
    point3d u={x,y,0.}, *urot=NULL;	/* original, rotated coords */
    /* ---
     * rotate pixel
     * --------------- */
    if ( pixval != bgpixel )		/* if not a background pixel */ 
      if ( (urot = matmult(rotmat,&u)) /* apply rotation matrix */
      !=   NULL ) {			/* succeeded */
        double dy=(+0.00), dx=(-0.25);	/*ad hoc rounding determined by tests*/
        int jrow = (int)(urot->y + midrow + dy); /* rotated row */
        int jcol = (int)(urot->x + midcol + dx); /* rotated col */
        if ( jrow>=0 && jrow<height	/* check rotated pixel coords */
        &&   jcol>=0 && jcol<width ) {	/* rotated pixel in bounds */
          setpixel(rotp,jrow,jcol,pixval); } /* rotated pixel in place */
        } /* --- end-of-if(urot!=NULL) --- */
    } /* --- end-of-for(icol) --- */
  } /* --- end-of-for(irow) --- */
/* ---
 * end-of-job
 * ------------- */
end_of_job:
  return ( rotp );			/* return rotated raster to caller */
} /* --- end-of-function rastrot3d() --- */


/* ==========================================================================
 * Function:	rastmag ( rp, magstep )
 * Purpose:	magnifies rp by integer magstep,
 *		e.g., double-height and double-width if magstep=2
 * --------------------------------------------------------------------------
 * Arguments:	rp (I)		ptr to raster struct to be "magnified"
 *		magstep (I)	int containing magnification scale,
 *				e.g., 2 to double the width and height of rp
 * --------------------------------------------------------------------------
 * Returns:	( raster * )	ptr to new raster magnified relative to rp,
 *				or NULL for any error.
 * --------------------------------------------------------------------------
 * Notes:     o
 * ======================================================================= */
/* --- entry point --- */
raster	*rastmag ( raster *rp, int magstep )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
raster	*new_raster(), *magnified=NULL;	/* magnified raster back to caller */
int	height = rp->height, irow,	/* height, row index */
	width = rp->width, icol,	/* width, column index */
	mrow = 0, mcol = 0,		/* dup pixels magstep*magstep times*/
	pixsz = rp->pixsz;		/* #bits per pixel */
/* -------------------------------------------------------------------------
check args
-------------------------------------------------------------------------- */
if ( rp == NULL ) goto end_of_job;	/* no input raster supplied */
if ( magstep<1 || magstep>10 ) goto end_of_job; /* sanity check */
/* -------------------------------------------------------------------------
allocate magnified raster and fill it
-------------------------------------------------------------------------- */
/* --- allocate magnified raster with magstep*width, magstep*height --- */
if ( (magnified = new_raster(magstep*width,magstep*height,pixsz))/*allocate*/
!=   NULL )				/* check that allocation succeeded */
  /* --- fill reflected raster --- */
  for ( irow=0; irow<height; irow++ )	/* for each row of rp */
    for ( mrow=0; mrow<magstep; mrow++ ) /* dup row magstep times */
      for ( icol=0; icol<width; icol++ ) /* and for each column of rp */
        for ( mcol=0; mcol<magstep; mcol++ ) { /* dup col magstep times */
         int value = getpixel(rp,irow,icol);
	 int row1 = irow*magstep, col1 = icol*magstep;
         setpixel(magnified,(row1+mrow),(col1+mcol),value); }
end_of_job:
  return ( magnified );			/*return magnified raster to caller*/
} /* --- end-of-function rastmag() --- */


/* ==========================================================================
 * Function:	bytemapmag ( bytemap, width, height, magstep )
 * Purpose:	magnifies a bytemap by integer magstep,
 *		e.g., double-height and double-width if magstep=2
 * --------------------------------------------------------------------------
 * Arguments:	bytemap (I)	intbyte * ptr to byte map to be "magnified"
 *		width (I)	int containing #cols in original bytemap
 *		height (I)	int containing #rows in original bytemap
 *		magstep (I)	int containing magnification scale,
 *				e.g., 2 to double the width and height of rp
 * --------------------------------------------------------------------------
 * Returns:	( intbyte * )	ptr to new bytemap magnified relative to
 *				original bytemap, or NULL for any error.
 * --------------------------------------------------------------------------
 * Notes:     o	Apply EPX/Scale2x/AdvMAME2x  for magstep 2,
 *		and Scale3x/AdvMAME3x  for magstep 3,
 *		as described by http://en.wikipedia.org/wiki/2xSaI
 * ======================================================================= */
/* --- entry point --- */
intbyte	*bytemapmag ( intbyte *bytemap, int width, int height, int magstep )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
intbyte	*magnified=NULL;		/* magnified bytemap back to caller*/
int	irow, icol,			/* original height, width indexes */
	mrow=0, mcol=0;			/* dup bytes magstep*magstep times */
int	imap = (-1),			/* original bytemap[] index */
	byteval = 0;			/* byteval=bytemap[imap] */
int	isAdvMAME = 1;			/* true to apply AdvMAME2x and 3x */
int	icell[10],			/* bytemap[] nearest neighbors */
	bmmdiff = 64;			/* nearest neighbor diff allowed */
#define	bmmeq(i,j) ((absval((icell[i]-icell[j]))<=bmmdiff)) /*approx equal*/
/* -------------------------------------------------------------------------
check args
-------------------------------------------------------------------------- */
if ( bytemap == NULL ) goto end_of_job;	/* no input bytemap supplied */
if ( width<1 || height<1 ) goto end_of_job; /* invalid bytemap dimensions */
if ( width*height>100000 ) goto end_of_job; /* sanity check */
if ( magstep<1 || magstep>10 ) goto end_of_job; /* sanity check */
/* -------------------------------------------------------------------------
allocate magnified bytemap and fill it
-------------------------------------------------------------------------- */
/* --- allocate bytemap for magstep*width, magstep*height --- */
if ( (magnified = (intbyte *)(malloc(magstep*width*magstep*height)))/*alloc*/
!=   NULL )				/* check that allocation succeeded */
  /* --- fill reflected raster --- */
  for ( irow=0; irow<height; irow++ )	/* for each row of bytemap */
   for ( icol=0; icol<width; icol++ ) { /* and for each column of bytemap */
    int imag1 = (icol + irow*(width*magstep))*magstep; /*upper-left corner*/
    imap++;				/* bump bytemap[] index */
    byteval = (int)(bytemap[imap]);	/* grayscale value at this pixel */
    for ( mrow=0; mrow<magstep; mrow++ ) /* dup row magstep times */
     for ( mcol=0; mcol<magstep; mcol++ ) { /* dup col magstep times */
      int idup = mcol + mrow*(width*magstep); /* offset from imag1 */
      int imag = imag1+idup;		/* adjust magnified[imag] */
      magnified[imag] = (intbyte)(byteval);
      /* --- apply AdvMAME2x and 3x (if desired) --- */
      if ( isAdvMAME ) {		/* AdvMAME2x and 3x wanted */
       int mcell = 1 + mcol + magstep*mrow; /*1,2,3,4 or 1,2,3,4,5,6,7,8,9*/
       icell[5]= byteval,		/* center cell of 3x3 bytemap[] */
       icell[4]= (icol>0?(int)(bytemap[imap-1]):byteval), /*left of center*/
       icell[6]= (icol<width?(int)(bytemap[imap+1]):byteval), /*right*/
       icell[2]= (irow>0?(int)(bytemap[imap-width]):byteval),/*above center*/
       icell[8]= (irow<height?(int)(bytemap[imap+width]):byteval), /*below*/
       icell[1]= (irow>0&&icol>0?(int)(bytemap[imap-width-1]):byteval),
       icell[3]= (irow>0&&icol<width?(int)(bytemap[imap-width+1]):byteval),
       icell[7]= (irow<height&&icol>0?(int)(bytemap[imap+width-1]):byteval),
      icell[9]=(irow<height&&icol<width?(int)(bytemap[imap+width+1]):byteval);
       switch ( magstep ) {		/* 2x magstep=2, 3x magstep=3 */
        default: break;			/* no AdvMAME at other magsteps */
        case 2:				/* AdvMAME2x */
         if ( mcell == 1 )
           if ( bmmeq(4,2) && !bmmeq(4,8) && !bmmeq(2,6) )
             magnified[imag] = icell[2];
         if ( mcell == 2 )
           if ( bmmeq(2,6) && !bmmeq(2,4) && !bmmeq(6,8) )
             magnified[imag] = icell[6];
         if ( mcell == 4 )
           if ( bmmeq(6,8) && !bmmeq(6,2) && !bmmeq(8,4) )
             magnified[imag] = icell[8];
         if ( mcell == 3 )
           if ( bmmeq(8,4) && !bmmeq(8,6) && !bmmeq(4,2) )
             magnified[imag] = icell[4];
         break;
        case 3:				/* AdvMAME3x */
         if ( mcell == 1 )
           if ( bmmeq(4,2) && !bmmeq(4,8) && !bmmeq(2,6) )
             magnified[imag] = icell[4];
         if ( mcell == 2 )
           if ( (bmmeq(4,2) && !bmmeq(4,8) && !bmmeq(2,6) && !bmmeq(5,3))
             || (bmmeq(2,6) && !bmmeq(2,4) && !bmmeq(6,8) && !bmmeq(5,1)) )
             magnified[imag] = icell[2];
         if ( mcell == 3 )
           if ( bmmeq(2,6) && !bmmeq(2,4) && !bmmeq(6,8) )
             magnified[imag] = icell[6];
         if ( mcell == 4 )
           if ( (bmmeq(8,4) && !bmmeq(8,6) && !bmmeq(4,2) && !bmmeq(5,1))
             || (bmmeq(4,2) && !bmmeq(4,8) && !bmmeq(2,6) && !bmmeq(5,7)) )
             magnified[imag] = icell[4];
         if ( mcell == 6 )
           if ( (bmmeq(2,6) && !bmmeq(2,4) && !bmmeq(6,8) && !bmmeq(5,9))
             || (bmmeq(6,8) && !bmmeq(6,2) && !bmmeq(8,4) && !bmmeq(5,3)) )
             magnified[imag] = icell[6];
         if ( mcell == 7 )
           if ( bmmeq(8,4) && !bmmeq(8,6) && !bmmeq(4,2) )
             magnified[imag] = icell[4];
         if ( mcell == 8 )
           if ( (bmmeq(6,8) && !bmmeq(6,2) && !bmmeq(8,4) && !bmmeq(5,7))
             || (bmmeq(8,4) && !bmmeq(8,6) && !bmmeq(4,2) && !bmmeq(5,9)) )
             magnified[imag] = icell[8];
         if ( mcell == 9 )
           if ( bmmeq(6,8) && !bmmeq(6,2) && !bmmeq(8,4) )
             magnified[imag] = icell[6];
         break;
        } } /* --- end-of-switch(magstep) --- */
      } /* --- end-of-for(mrow,mcol) --- */
    } /* --- end-of-for(irow,icol) --- */
end_of_job:
  return ( magnified );			/*return magnified raster to caller*/
} /* --- end-of-function bytemapmag() --- */


/* ==========================================================================
 * Function:	rastref ( rp, axis )
 * Purpose:	reflects rp, horizontally about y-axis |_ becomes _| if axis=1
 *		or vertically about x-axis M becomes W if axis=2.
 * --------------------------------------------------------------------------
 * Arguments:	rp (I)		ptr to raster struct to be reflected
 *		axis (I)	int containing 1 for horizontal reflection,
 *				or 2 for vertical
 * --------------------------------------------------------------------------
 * Returns:	( raster * )	ptr to new raster reflected relative to rp,
 *				or NULL for any error.
 * --------------------------------------------------------------------------
 * Notes:     o
 * ======================================================================= */
/* --- entry point --- */
raster	*rastref ( raster *rp, int axis )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
raster	*new_raster(), *reflected=NULL;	/* reflected raster back to caller */
int	height = rp->height, irow,	/* height, row index */
	width = rp->width, icol,	/* width, column index */
	pixsz = rp->pixsz;		/* #bits per pixel */
/* -------------------------------------------------------------------------
allocate reflected raster and fill it
-------------------------------------------------------------------------- */
/* --- allocate reflected raster with same width, height --- */
if ( axis==1 || axis==2 )		/* first validate axis arg */
 if ( (reflected = new_raster(width,height,pixsz)) /* same width, height */
 !=   NULL )				/* check that allocation succeeded */
  /* --- fill reflected raster --- */
  for ( irow=0; irow<height; irow++ )	/* for each row of rp */
    for ( icol=0; icol<width; icol++ ) { /* and each column of rp */
      int value = getpixel(rp,irow,icol);
      if ( axis == 1 ) { setpixel(reflected,irow,width-1-icol,value); }
      if ( axis == 2 ) { setpixel(reflected,height-1-irow,icol,value); } }
return ( reflected );			/*return reflected raster to caller*/
} /* --- end-of-function rastref() --- */


/* ==========================================================================
 * Function:	rastput ( target, source, top, left, isopaque )
 * Purpose:	Overlays source onto target,
 *		with the 0,0-bit of source onto the top,left-bit of target.
 * --------------------------------------------------------------------------
 * Arguments:	target (I)	ptr to target raster struct
 *		source (I)	ptr to source raster struct
 *		top (I)		int containing 0 ... target->height - 1
 *		left (I)	int containing 0 ... target->width - 1
 *		isopaque (I)	int containing false (zero) to allow
 *				original 1-bits of target to "show through"
 *				0-bits of source.
 * --------------------------------------------------------------------------
 * Returns:	( int )		1 if completed successfully,
 *				or 0 otherwise (for any error).
 * --------------------------------------------------------------------------
 * Notes:
 * ======================================================================= */
/* --- entry point --- */
int	rastput ( raster *target, raster *source,
		int top, int left, int isopaque )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
int	irow, icol,		/* indexes over source raster */
	twidth=target->width, theight=target->height, /*target width,height*/
	tpix, ntpix = twidth*theight; /* #pixels in target */
int	isfatal = 0,		/* true to abend on out-of-bounds error */
	isstrict = 0/*1*/,	/* true for strict bounds check - no "wrap"*/
	isokay = 1;		/* true if no pixels out-of-bounds */
/* -------------------------------------------------------------------------
superimpose source onto target, one bit at a time
-------------------------------------------------------------------------- */
if ( isstrict && (top<0 || left<0) )		/* args fail strict test */
 isokay = 0;					/* so just return error */
else
 for ( irow=0; irow<source->height; irow++ )	/* for each scan line */
  {
  tpix = (top+irow)*target->width + left - 1;	/*first target pixel (-1)*/
  for ( icol=0; icol<source->width; icol++ )	/* each pixel in scan line */
    {
    int svalue = getpixel(source,irow,icol);	/* source pixel value */
    ++tpix;					/* bump target pixel */
    if ( msgfp!=NULL && msglevel>=9999 )	/* debugging output */
      {	fprintf(msgfp,"rastput> tpix,ntpix=%d,%d top,irow,theight=%d,%d,%d "
	"left,icol,twidth=%d,%d,%d\n", tpix,ntpix, top,irow,theight,
	left,icol,twidth);  fflush(msgfp); }
    if ( tpix >= ntpix				/* bounds check failed */
    ||   (isstrict && (irow+top>=theight || icol+left>=twidth)) )
      {	isokay = 0;				/* reset okay flag */
	if ( isfatal ) goto end_of_job;		/* abort if error is fatal */
	else break; }				/*or just go on to next row*/
    if ( tpix >= 0 )				/* bounds check okay */
     if ( svalue!=0 || isopaque ) {		/*got dark or opaque source*/
      setpixel(target,irow+top,icol+left,svalue); }/*overlay source on targ*/
    } /* --- end-of-for(icol) --- */
  } /* --- end-of-for(irow) --- */
/* -------------------------------------------------------------------------
Back to caller with 1=okay, 0=failed.
-------------------------------------------------------------------------- */
end_of_job:
  return ( isokay /*isfatal? (tpix<ntpix? 1:0) : 1*/ );
} /* --- end-of-function rastput() --- */


/* ==========================================================================
 * Function:	rastcompose ( sp1, sp2, offset2, isalign, isfree )
 * Purpose:	Overlays sp2 on top of sp1, leaving both unchanged
 *		and returning a newly-allocated composite subraster.
 *		Frees/deletes input sp1 and/or sp2 depending on value
 *		of isfree (0=none, 1=sp1, 2=sp2, 3=both).
 * --------------------------------------------------------------------------
 * Arguments:	sp1 (I)		subraster *  to "underneath" subraster,
 *				whose baseline is preserved
 *		sp2 (I)		subraster *  to "overlaid" subraster
 *		offset2 (I)	int containing 0 or number of pixels
 *				to horizontally shift sp2 relative to sp1,
 *				either positive (right) or negative
 *		isalign (I)	int containing 1 to align baselines,
 *				or 0 to vertically center sp2 over sp1.
 *				For isalign=2, images are vertically
 *				centered, but then adjusted by \raisebox
 *				lifts, using global variables rastlift1
 *				for sp1 and rastlift for sp2.
 *		isfree (I)	int containing 1=free sp1 before return,
 *				2=free sp2, 3=free both, 0=free none.
 * --------------------------------------------------------------------------
 * Returns:	( subraster * )	pointer to constructed subraster
 *				or  NULL for any error
 * --------------------------------------------------------------------------
 * Notes:     o	The top-left corner of each raster box has coords (0,0),
 *		down to (h-1,w-1) for a box of height h and width w.
 *	      o	A raster's baseline, b, is typically 0 <= b < h.
 *		But b can actually go out-of-bounds, b>=h or b<0, for
 *		an image additionally lifted (b>=h) or lowered (b<0)
 *		with respect to the surrounding expression.
 *	      o	Note that b=h-1 means no descenders and the bottom
 *		of the symbol rests exactly on the baseline,
 *		whereas b=0 means the top pixel of the symbol rests
 *		on the baseline, and all other pixels are descenders.
 *	      o	The composite raster is constructed as follows...
 *		The base image is labelled height h1 and baseline b1,
 *		the overlay h2 and b2, and the composite H and B.
 *		     base       overlay
 *	    --- +------------------------+ ---   For the overlay to be
 *	     ^  |   ^        +----------+|  ^    vertically centered with
 *	     |  |   |        |          ||  |    respect to the base,
 *	     |  |   |B-b1    |          ||  |      B - b1 = H-B -(h1-b1), so
 *	     |  |   v        |          ||  |      2*B = H-h1 + 2*b1
 *	     |  |+----------+|          ||  |      B = b1 + (H-h1)/2
 *	     B  ||  ^    ^  ||          ||  |    And when the base image is
 *	     |  ||  |    |  ||          ||  |    bigger, H=h1 and B=b1 is
 *	     |  ||  b1   |  ||          ||  |    the obvious correct answer.
 *	     |  ||  |    h1 ||          || H=h2
 *	     v  ||  v    |  ||          ||  |
 *    ----------||-------|--||          ||--|--------
 *    baseline  || h1-b1 v  || overlay  ||  |
 *    for base  |+----------+| baseline ||  |
 *    and com-  |   ^        | ignored  ||  |
 *    posite    |   |H-B-    |----------||  |
 *		|   | (h1-b1)|          ||  |
 *		|   v        +----------+|  v
 *		+------------------------+ ---
 * ======================================================================= */
/* --- entry point --- */
subraster *rastcompose ( subraster *sp1, subraster *sp2, int offset2,
			int isalign, int isfree )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
subraster *new_subraster(), *sp=(subraster *)NULL; /* returned subraster */
raster	*rp=(raster *)NULL;		/* new composite raster in sp */
int	delete_subraster();		/* in case isfree non-zero */
int	rastput();			/*place sp1,sp2 in composite raster*/
int	base1   = sp1->baseline,	/*baseline for underlying subraster*/
	height1 = (sp1->image)->height,	/* height for underlying subraster */
	width1  = (sp1->image)->width,	/* width for underlying subraster */
	pixsz1  = (sp1->image)->pixsz,	/* pixsz for underlying subraster */
	base2   = sp2->baseline,	/*baseline for overlaid subraster */
	height2 = (sp2->image)->height,	/* height for overlaid subraster */
	width2  = (sp2->image)->width,	/* width for overlaid subraster */
	pixsz2  = (sp2->image)->pixsz;	/* pixsz for overlaid subraster */
int	height  = max2(height1,height2), /*composite height if sp2 centered*/
	base    = base1 + (height-height1)/2, /* and composite baseline */
	tlc2    = (height-height2)/2,	/* top-left corner for overlay */
	width=0, pixsz=0;		/* other params for composite */
int	lift1   = rastlift1,		/* vertical \raisebox lift for sp1 */
	lift2   = rastlift;		/* vertical \raisebox lift for sp2 */
/* -------------------------------------------------------------------------
Initialization
-------------------------------------------------------------------------- */
/* --- determine height, width and baseline of composite raster --- */
switch ( isalign ) {
  default:
  case 0:				/* centered, baselines not aligned */
    height = max2(height1,height2);	/* max height */
    base   = base1 + (height-height1)/2; /* baseline for sp1 */
    break;
  case 1:				/* baselines of sp1,sp2 aligned */
    height = max2(base1+1,base2+1)	/* max height above baseline */
           + max2(height1-base1-1,height2-base2-1); /*+max descending below*/
    base   = max2(base1,base2);		/* max space above baseline */
    break;
  case 2:				/* centered +/- \raisebox lifts */
    base1 -= lift1;  base2 -= lift2;	/* reset to unlifted images */
    /* --- start with default for centered, unlifted images --- */
    height2 += 2*absval(lift2);		/* "virtual" height of overlay */
    height = max2(height1,height2);	/* max height */
    base   = base1 + (height-height1)/2; /* baseline for sp1 */
    tlc2   = (height-height2)/2		/* top-left corner for overlay */
           + (lift2>=0?0:2*absval(lift2)); /* "reflect" overlay below base */
    break;
  } /* --- end-of-switch(isalign) --- */
width = max2(width1,width2+abs(offset2)); /* max width */
pixsz = max2(pixsz1,pixsz2);		/* bitmap,bytemap becomes bytemap */
/* -------------------------------------------------------------------------
allocate concatted composite subraster
-------------------------------------------------------------------------- */
/* --- allocate returned subraster (and then initialize it) --- */
if ( (sp=new_subraster(width,height,pixsz)) /* allocate new subraster */
==   (subraster *)NULL ) goto end_of_job; /* failed, so quit */
/* --- initialize subraster parameters --- */
sp->type = IMAGERASTER;			/* image */
sp->baseline = base;			/* composite baseline */
sp->size = sp1->size;			/* underlying char is sp1 */
if ( isalign == 2 ) sp->baseline += lift1; /* adjust baseline */
/* --- extract raster from subraster --- */
rp = sp->image;				/* raster allocated in subraster */
/* -------------------------------------------------------------------------
overlay sp1 and sp2 in new composite raster
-------------------------------------------------------------------------- */
switch ( isalign ) {
  default:
  case 0:				/* centered, baselines not aligned */
    rastput (rp, sp1->image, base-base1, (width-width1)/2, 1);	/*underlying*/
    rastput (rp, sp2->image, (height-height2)/2,		/*overlaid*/
		(width-width2)/2+offset2, 0);
    break;
  case 1:				/* baselines of sp1,sp2 aligned */
    rastput (rp, sp1->image, base-base1, (width-width1)/2, 1);	/*underlying*/
    rastput (rp, sp2->image, base-base2,			/*overlaid*/
		(width-width2)/2+offset2, 0);
    break;
  case 2: if(1){			/* centered +/- \raisebox lifts */
    rastput (rp, sp1->image, base-base1, (width-width1)/2, 1);
    rastput (rp, sp2->image, tlc2, (width-width2)/2+offset2, 0); }
    break;
  } /* --- end-of-switch(isalign) --- */
/* -------------------------------------------------------------------------
free input if requested
-------------------------------------------------------------------------- */
if ( isfree > 0 )			/* caller wants input freed */
  { if ( isfree==1 || isfree>2 ) delete_subraster(sp1);	/* free sp1 */
    if ( isfree >= 2 ) delete_subraster(sp2); }		/* and/or sp2 */
/* -------------------------------------------------------------------------
Back to caller with pointer to concatted subraster or with null for error
-------------------------------------------------------------------------- */
end_of_job:
  return ( sp );			/* back with subraster or null ptr */
} /* --- end-of-function rastcompose() --- */


/* ==========================================================================
 * Function:	rastcat ( sp1, sp2, isfree )
 * Purpose:	"Concatanates" subrasters sp1||sp2, leaving both unchanged
 *		and returning a newly-allocated subraster.
 *		Frees/deletes input sp1 and/or sp2 depending on value
 *		of isfree (0=none, 1=sp1, 2=sp2, 3=both).
 * --------------------------------------------------------------------------
 * Arguments:	sp1 (I)		subraster *  to left-hand subraster
 *		sp2 (I)		subraster *  to right-hand subraster
 *		isfree (I)	int containing 1=free sp1 before return,
 *				2=free sp2, 3=free both, 0=free none.
 * --------------------------------------------------------------------------
 * Returns:	( subraster * )	pointer to constructed subraster sp1||sp2
 *				or  NULL for any error
 * --------------------------------------------------------------------------
 * Notes:
 * ======================================================================= */
/* --- entry point --- */
subraster *rastcat ( subraster *sp1, subraster *sp2, int isfree )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
subraster *new_subraster(), *sp=(subraster *)NULL; /* returned subraster */
raster	*rp=(raster *)NULL;		/* new concatted raster */
int	delete_subraster();		/* in case isfree non-zero */
int	rastput();			/*place sp1,sp2 in concatted raster*/
int	type_raster();			/* debugging display */
int	base1   = sp1->baseline,	/*baseline for left-hand subraster*/
	height1 = (sp1->image)->height,	/* height for left-hand subraster */
	width1  = (sp1->image)->width,	/* width for left-hand subraster */
	pixsz1  = (sp1->image)->pixsz,	/* pixsz for left-hand subraster */
	type1   = sp1->type,		/* image type for left-hand */
	base2   = sp2->baseline,	/*baseline for right-hand subraster*/
	height2 = (sp2->image)->height,	/* height for right-hand subraster */
	width2  = (sp2->image)->width,	/* width for right-hand subraster */
	pixsz2  = (sp2->image)->pixsz,	/* pixsz for right-hand subraster */
	type2   = sp2->type;		/* image type for right-hand */
int	height=0, width=0, pixsz=0, base=0; /*concatted sp1||sp2 composite*/
int	issmash = (smashmargin!=0?1:0),	/* true to "squash" sp1||sp2 */
	isopaque = (issmash?0:1),	/* not oppaque if smashing */
	rastsmash(), isblank=0, nsmash=0, /* #cols to smash */
	oldsmashmargin = smashmargin,	/* save original smashmargin */
	oldblanksymspace = blanksymspace, /* save original blanksymspace */
	oldnocatspace = isnocatspace;	/* save original isnocatspace */
mathchardef *symdef1 = sp1->symdef,	/*mathchardef of last left-hand char*/
	*symdef2 = sp2->symdef;		/* mathchardef of right-hand char */
int	class1 = (symdef1==NULL?ORDINARY:symdef1->class), /* symdef->class */
	class2 = (symdef2==NULL?ORDINARY:symdef2->class), /* or default */
	smash1 = (symdef1!=NULL)&&(class1==ORDINARY||class1==VARIABLE||
		  class1==OPENING||class1==CLOSING||class1==PUNCTION),
	smash2 = (symdef2!=NULL)&&(class2==ORDINARY||class2==VARIABLE||
		  class2==OPENING||class2==CLOSING||class2==PUNCTION),
	space = fontsize/2+1;		/* #cols between sp1 and sp2 */
int	isfrac = (type1 == FRACRASTER	/* sp1 is a \frac */
		  && class2 == PUNCTION); /* and sp2 is punctuation */
/* -------------------------------------------------------------------------
Initialization
-------------------------------------------------------------------------- */
/* --- determine inter-character space from character class --- */
if ( !isstring )
  space = max2(2,(symspace[class1][class2] + fontsize-3)); /* space */
else space = 1;				/* space for ascii string */
if ( isnocatspace > 0 ) {		/* spacing explicitly turned off */
  space = 0;				/* reset space */
  isnocatspace--; }			/* and decrement isnocatspace flag */
if ( 0 && sp1->type == BLANKSIGNAL ) space=0; /*implicitly turn off spacing*/
if ( sp1->type==BLANKSIGNAL && sp2->type==BLANKSIGNAL ) /* both blank */
  space = 0;				/* no extra space between spaces */
if ( sp2->type != BLANKSIGNAL )		/* not a blank space signal */
  if ( blanksymspace != 0 ) {		/* and we have a space adjustment */
    space = max2(0,space+blanksymspace); /* adjust as much as possible */
    blanksymspace = 0; }		/* and reset adjustment */
if ( msgfp!=NULL && msglevel>=999 )	/* display space results */
  { fprintf(msgfp,"rastcat> space=%d, blanksymspace=%d, isnocatspace=%d\n",
    space,oldblanksymspace,oldnocatspace);  fflush(msgfp); }
/* --- determine smash --- */
if ( !isstring && !isfrac )		/* don't smash strings or \frac's */
 if ( issmash ) {			/* raster smash wanted */
   int	maxsmash = rastsmash(sp1,sp2),	/* calculate max smash space */
	margin = smashmargin;		/* init margin without delta */
   if ( (1 && smash1 && smash2)		/* concatanating two chars */
   ||   (1 && type1!=IMAGERASTER && type2!=IMAGERASTER
	   && type1!=FRACRASTER  && type2!=FRACRASTER ) )
     /*maxsmash = 0;*/			/* turn off smash */
     margin = max2(space-1,0);		/* force small smashmargin */
   else					/* adjust for delta if images */
     if ( issmashdelta )		/* smashmargin is a delta value */
       margin += fontsize;		/* add displaystyle base to margin */
   if ( maxsmash == blanksignal )	/* sp2 is intentional blank */
     isblank = 1;			/* set blank flag signal */
   else					/* see how much extra space we have*/
     if ( maxsmash > margin )		/* enough space for adjustment */
       nsmash = maxsmash-margin;	/* make adjustment */
   if ( msgfp!=NULL && msglevel>=99 )	/* display smash results */
     { fprintf(msgfp,"rastcat> maxsmash=%d, margin=%d, nsmash=%d\n",
       maxsmash,margin,nsmash);
       fprintf(msgfp,"rastcat> type1=%d,2=%d, class1=%d,2=%d\n", type1,type2,
       (symdef1==NULL?-999:class1),(symdef2==NULL?-999:class2));
       fflush(msgfp); }
   } /* --- end-of-if(issmash) --- */
/* --- determine height, width and baseline of composite raster --- */
if ( !isstring )
 { height = max2(base1+1,base2+1)	/* max height above baseline */
          + max2(height1-base1-1,height2-base2-1); /*+ max descending below*/
   width  = width1+width2 + space-nsmash; /*add widths and space-smash*/
   width  = max3(width,width1,width2); } /* don't "over-smash" composite */
else					/* ascii string */
 { height = 1;				/* default */
   width  = width1 + width2 + space - 1; } /* no need for two nulls */
pixsz  = max2(pixsz1,pixsz2);		/* bitmap||bytemap becomes bytemap */
base   = max2(base1,base2);		/* max space above baseline */
if ( msgfp!=NULL && msglevel>=9999 )	/* display components */
  { fprintf(msgfp,"rastcat> Left-hand ht,width,pixsz,base = %d,%d,%d,%d\n",
    height1,width1,pixsz1,base1);
    type_raster(sp1->image,msgfp);	/* display left-hand raster */
    fprintf(msgfp,"rastcat> Right-hand ht,width,pixsz,base = %d,%d,%d,%d\n",
    height2,width2,pixsz2,base2);
    type_raster(sp2->image,msgfp);	/* display right-hand raster */
    fprintf(msgfp,
    "rastcat> Composite ht,width,smash,pixsz,base = %d,%d,%d,%d,%d\n",
    height,width,nsmash,pixsz,base);
    fflush(msgfp); }			/* flush msgfp buffer */
/* -------------------------------------------------------------------------
allocate concatted composite subraster
-------------------------------------------------------------------------- */
/* --- allocate returned subraster (and then initialize it) --- */
if ( msgfp!=NULL && msglevel>=9999 )
  { fprintf(msgfp,"rastcat> calling new_subraster(%d,%d,%d)\n",
    width,height,pixsz); fflush(msgfp); }
if ( (sp=new_subraster(width,height,pixsz)) /* allocate new subraster */
==   (subraster *)NULL )		/* failed */
  { if ( msgfp!=NULL && msglevel>=1 )	/* report failure */
      {	fprintf(msgfp,"rastcat> new_subraster(%d,%d,%d) failed\n",
	width,height,pixsz); fflush(msgfp); }
    goto end_of_job; }			/* failed, so quit */
/* --- initialize subraster parameters --- */
/* sp->type = (!isstring?STRINGRASTER:ASCIISTRING); */  /*concatted string*/
if ( !isstring )
  sp->type = /*type2;*//*(type1==type2?type2:IMAGERASTER);*/
	(type2!=CHARASTER? type2 :
	(type1!=CHARASTER&&type1!=BLANKSIGNAL
	 &&type1!=FRACRASTER?type1:IMAGERASTER));
else
  sp->type = ASCIISTRING;		/* concatted ascii string */
sp->symdef = symdef2;			/* rightmost char is sp2 */
sp->baseline = base;			/* composite baseline */
sp->size = sp2->size;			/* rightmost char is sp2 */
if ( isblank )				/* need to propagate blanksignal */
  sp->type = blanksignal;		/* may not be completely safe??? */
/* --- extract raster from subraster --- */
rp = sp->image;				/* raster allocated in subraster */
/* -------------------------------------------------------------------------
overlay sp1 and sp2 in new composite raster
-------------------------------------------------------------------------- */
if ( msgfp!=NULL && msglevel>=9999 )
  { fprintf(msgfp,"rastcat> calling rastput() to concatanate left||right\n");
    fflush(msgfp); }			/* flush msgfp buffer */
if ( !isstring )
 rastput (rp, sp1->image, base-base1,	/* overlay left-hand */
 max2(0,nsmash-width1), 1);		/* plus any residual smash space */
else
 memcpy(rp->pixmap,(sp1->image)->pixmap,width1-1);  /*init left string*/
if ( msgfp!=NULL && msglevel>=9999 )
  { type_raster(sp->image,msgfp);	/* display composite raster */
    fflush(msgfp); }			/* flush msgfp buffer */
if ( !isstring )
 { int	fracbase = ( isfrac?		/* baseline for punc after \frac */
	max2(fraccenterline,base2):base ); /*adjust baseline or use original*/
   rastput (rp, sp2->image, fracbase-base2, /* overlay right-hand */
   max2(0,width1+space-nsmash), isopaque); /* minus any smashed space */
   if ( 1 && type1 == FRACRASTER	/* we're done with \frac image */
   &&   type2 != FRACRASTER )		/* unless we have \frac\frac */
     fraccenterline = NOVALUE;		/* so reset centerline signal */
   if ( fraccenterline != NOVALUE )	/* sp2 is a fraction */
     fraccenterline += (base-base2); }	/* so adjust its centerline */
else
 { strcpy((char *)(rp->pixmap)+width1-1+space,(char *)((sp2->image)->pixmap));
   ((char *)(rp->pixmap))[width1+width2+space-2] = '\000'; } /*null-term*/
if ( msgfp!=NULL && msglevel>=9999 )
  { type_raster(sp->image,msgfp);	/* display composite raster */
    fflush(msgfp); }			/* flush msgfp buffer */
/* -------------------------------------------------------------------------
free input if requested
-------------------------------------------------------------------------- */
if ( isfree > 0 )			/* caller wants input freed */
  { if ( isfree==1 || isfree>2 ) delete_subraster(sp1);	/* free sp1 */
    if ( isfree >= 2 ) delete_subraster(sp2); }		/* and/or sp2 */
/* -------------------------------------------------------------------------
Back to caller with pointer to concatted subraster or with null for error
-------------------------------------------------------------------------- */
end_of_job:
  smashmargin = oldsmashmargin;		/* reset original smashmargin */
  return ( sp );			/* back with subraster or null ptr */
} /* --- end-of-function rastcat() --- */


/* ==========================================================================
 * Function:	rastack ( sp1, sp2, base, space, iscenter, isfree )
 * Purpose:	Stack subrasters sp2 atop sp1, leaving both unchanged
 *		and returning a newly-allocated subraster,
 *		whose baseline is sp1's if base=1, or sp2's if base=2.
 *		Frees/deletes input sp1 and/or sp2 depending on value
 *		of isfree (0=none, 1=sp1, 2=sp2, 3=both).
 * --------------------------------------------------------------------------
 * Arguments:	sp1 (I)		subraster *  to lower subraster
 *		sp2 (I)		subraster *  to upper subraster
 *		base (I)	int containing 1 if sp1 is baseline,
 *				or 2 if sp2 is baseline.
 *		space (I)	int containing #rows blank space inserted
 *				between sp1's image and sp2's image.
 *		iscenter (I)	int containing 1 to center both sp1 and sp2
 *				in stacked array, 0 to left-justify both
 *		isfree (I)	int containing 1=free sp1 before return,
 *				2=free sp2, 3=free both, 0=free none.
 * --------------------------------------------------------------------------
 * Returns:	( subraster * )	pointer to constructed subraster sp2 atop sp1
 *				or  NULL for any error
 * --------------------------------------------------------------------------
 * Notes:
 * ======================================================================= */
/* --- entry point --- */
subraster *rastack ( subraster *sp1, subraster *sp2,
			int base, int space, int iscenter, int isfree )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
subraster *new_subraster(), *sp=(subraster *)NULL; /* returned subraster */
raster	*rp=(raster *)NULL;		/* new stacked raster in sp */
int	delete_subraster();		/* in case isfree non-zero */
int	rastput();			/* place sp1,sp2 in stacked raster */
int	base1   = sp1->baseline,	/* baseline for lower subraster */
	height1 = (sp1->image)->height,	/* height for lower subraster */
	width1  = (sp1->image)->width,	/* width for lower subraster */
	pixsz1  = (sp1->image)->pixsz,	/* pixsz for lower subraster */
	base2   = sp2->baseline,	/* baseline for upper subraster */
	height2 = (sp2->image)->height,	/* height for upper subraster */
	width2  = (sp2->image)->width,	/* width for upper subraster */
	pixsz2  = (sp2->image)->pixsz;	/* pixsz for upper subraster */
int	height=0, width=0, pixsz=0, baseline=0;	/*for stacked sp2 atop sp1*/
mathchardef *symdef1 = sp1->symdef,	/* mathchardef of right lower char */
	*symdef2 = sp2->symdef;		/* mathchardef of right upper char */
/* -------------------------------------------------------------------------
Initialization
-------------------------------------------------------------------------- */
/* --- determine height, width and baseline of composite raster --- */
height   = height1 + space + height2;	/* sum of heights plus space */
width    = max2(width1,width2);		/* max width is overall width */
pixsz    = max2(pixsz1,pixsz2);		/* bitmap||bytemap becomes bytemap */
baseline = (base==1? height2+space+base1 : (base==2? base2 : 0));
/* -------------------------------------------------------------------------
allocate stacked composite subraster (with embedded raster)
-------------------------------------------------------------------------- */
/* --- allocate returned subraster (and then initialize it) --- */
if ( (sp=new_subraster(width,height,pixsz)) /* allocate new subraster */
==   (subraster *)NULL ) goto end_of_job; /* failed, so quit */
/* --- initialize subraster parameters --- */
sp->type = IMAGERASTER;			/* stacked rasters */
sp->symdef = (base==1? symdef1 : (base==2? symdef2 : NULL)); /* symdef */
sp->baseline = baseline;		/* composite baseline */
sp->size = (base==1? sp1->size : (base==2? sp2->size : NORMALSIZE)); /*size*/
/* --- extract raster from subraster --- */
rp = sp->image;				/* raster embedded in subraster */
/* -------------------------------------------------------------------------
overlay sp1 and sp2 in new composite raster
-------------------------------------------------------------------------- */
if ( iscenter == 1 )			/* center both sp1 and sp2 */
  { rastput (rp, sp2->image, 0, (width-width2)/2, 1);  /* overlay upper */
    rastput (rp, sp1->image, height2+space, (width-width1)/2, 1); } /*lower*/
else					/* left-justify both sp1 and sp2 */
  { rastput (rp, sp2->image, 0, 0, 1);  /* overlay upper */
    rastput (rp, sp1->image, height2+space, 0, 1); } /*lower*/
/* -------------------------------------------------------------------------
free input if requested
-------------------------------------------------------------------------- */
if ( isfree > 0 )			/* caller wants input freed */
  { if ( isfree==1 || isfree>2 ) delete_subraster(sp1);	/* free sp1 */
    if ( isfree>=2 ) delete_subraster(sp2); } /* and/or sp2 */
/* -------------------------------------------------------------------------
Back to caller with pointer to stacked subraster or with null for error
-------------------------------------------------------------------------- */
end_of_job:
  return ( sp );			/* back with subraster or null ptr */
} /* --- end-of-function rastack() --- */


/* ==========================================================================
 * Function:	rastile ( tiles, ntiles )
 * Purpose:	Allocate and build up a composite raster
 *		from the ntiles components/characters supplied in tiles.
 * --------------------------------------------------------------------------
 * Arguments:	tiles (I)	subraster *  to array of subraster structs
 *				describing the components and their locations
 *		ntiles (I)	int containing number of subrasters in tiles[]
 * --------------------------------------------------------------------------
 * Returns:	( raster * )	ptr to composite raster,
 *				or NULL for any error.
 * --------------------------------------------------------------------------
 * Notes:     o	The top,left corner of a raster is row=0,col=0
 *		with row# increasing as you move down,
 *		and col# increasing as you move right.
 *		Metafont numbers rows with the baseline=0,
 *		so the top row is a positive number that
 *		decreases as you move down.
 *	      o	rastile() is no longer used.
 *		It was used by an earlier rasterize() algorithm,
 *		and I've left it in place should it be needed again.
 *		But recent changes haven't been tested/exercised.
 * ======================================================================= */
/* --- entry point --- */
raster	*rastile ( subraster *tiles, int ntiles )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
raster	*new_raster(), *composite=(raster *)NULL;  /*raster back to caller*/
int	width=0, height=0, pixsz=0, /*width,height,pixsz of composite raster*/
	toprow=9999, rightcol=-999, /* extreme upper-right corner of tiles */
	botrow=-999, leftcol=9999;  /* extreme lower-left corner of tiles */
int	itile;			/* tiles[] index */
int	rastput();		/* overlay each tile in composite raster */
/* -------------------------------------------------------------------------
run through tiles[] to determine dimensions for composite raster
-------------------------------------------------------------------------- */
/* --- determine row and column bounds of composite raster --- */
for ( itile=0; itile<ntiles; itile++ )
  {
  subraster *tile = &(tiles[itile]);		/* ptr to current tile */
  /* --- upper-left corner of composite --- */
  toprow = min2(toprow, tile->toprow);
  leftcol = min2(leftcol, tile->leftcol);
  /* --- lower-right corner of composite --- */
  botrow = max2(botrow, tile->toprow + (tile->image)->height - 1);
  rightcol = max2(rightcol, tile->leftcol + (tile->image)->width  - 1);
  /* --- pixsz of composite --- */
  pixsz = max2(pixsz,(tile->image)->pixsz);
  } /* --- end-of-for(itile) --- */
/* --- calculate width and height from bounds --- */
width  = rightcol - leftcol + 1;
height = botrow - toprow + 1;
/* --- sanity check (quit if bad dimensions) --- */
if ( width<1 || height<1 ) goto end_of_job;
/* -------------------------------------------------------------------------
allocate composite raster, and embed tiles[] within it
-------------------------------------------------------------------------- */
/* --- allocate composite raster --- */
if ( (composite=new_raster(width,height,pixsz))	/*allocate composite raster*/
==   (raster *)NULL ) goto end_of_job;		/* and quit if failed */
/* --- embed tiles[] in composite --- */
for ( itile=0; itile<ntiles; itile++ )
  { subraster *tile = &(tiles[itile]);		/* ptr to current tile */
    rastput (composite, tile->image,		/* overlay tile image at...*/
      tile->toprow-toprow, tile->leftcol-leftcol, 1); } /*upper-left corner*/
/* -------------------------------------------------------------------------
Back to caller with composite raster (or null for any error)
-------------------------------------------------------------------------- */
end_of_job:
  return ( composite );			/* back with composite or null ptr */
} /* --- end-of-function rastile() --- */


/* ==========================================================================
 * Function:	rastsmash ( sp1, sp2 )
 * Purpose:	When concatanating sp1||sp2, calculate #pixels
 *		we can "smash sp2 left"
 * --------------------------------------------------------------------------
 * Arguments:	sp1 (I)		subraster *  to left-hand raster
 *		sp2 (I)		subraster *  to right-hand raster
 * --------------------------------------------------------------------------
 * Returns:	( int )		max #pixels we can smash sp1||sp2,
 *				or "blanksignal" if sp2 intentionally blank,
 *				or 0 for any error.
 * --------------------------------------------------------------------------
 * Notes:     o
 * ======================================================================= */
/* --- entry point --- */
int	rastsmash ( subraster *sp1, subraster *sp2 )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
int	nsmash = 0;			/* #pixels to smash sp1||sp2 */
int	base1   = sp1->baseline,	/*baseline for left-hand subraster*/
	height1 = (sp1->image)->height,	/* height for left-hand subraster */
	width1  = (sp1->image)->width,	/* width for left-hand subraster */
	base2   = sp2->baseline,	/*baseline for right-hand subraster*/
	height2 = (sp2->image)->height,	/* height for right-hand subraster */
	width2  = (sp2->image)->width;	/* width for right-hand subraster */
int	base = max2(base1,base2),	/* max ascenders - 1 above baseline*/
	top1=base-base1, top2=base-base2, /* top irow indexes for sp1, sp2 */
	bot1=top1+height1-1, bot2=top2+height2-1, /* bot irow indexes */
	height = max2(bot1,bot2)+1;	/* total height */
int	irow1=0,irow2=0, icol=0;	/* row,col indexes */
int	firstcol1[1025], nfirst1=0,	/* 1st sp1 col containing set pixel*/
	firstcol2[1025], nfirst2=0;	/* 1st sp2 col containing set pixel*/
int	smin=9999, xmin=9999,ymin=9999;	/* min separation (s=x+y) */
int	type_raster();			/* display debugging output */
/* -------------------------------------------------------------------------
find right edge of sp1 and left edge of sp2 (these will be abutting edges)
-------------------------------------------------------------------------- */
/* --- check args --- */
if ( isstring ) goto end_of_job;	/* ignore string rasters */
if ( 0 && istextmode ) goto end_of_job;	/* don't smash in text mode */
if ( height > 1023 ) goto end_of_job;	/* don't try to smash huge image */
if ( sp2->type == blanksignal )		/*blanksignal was propagated to us*/
  goto end_of_job;			/* don't smash intentional blank */
/* --- init firstcol1[], firstcol2[] --- */
for ( irow1=0; irow1<height; irow1++ )	/* for each row */
  firstcol1[irow1] = firstcol2[irow1] = blanksignal; /* signal empty rows */
/* --- set firstcol2[] indicating left edge of sp2 --- */
for ( irow2=top2; irow2<=bot2; irow2++ ) /* for each row inside sp2 */
  for ( icol=0; icol<width2; icol++ )	/* find first non-empty col in row */
    if ( getpixel(sp2->image,irow2-top2,icol) != 0 ) /* found a set pixel */
      {	firstcol2[irow2] = icol;	/* icol is #cols from left edge */
	nfirst2++;			/* bump #rows containing set pixels*/
	break; }			/* and go on to next row */
if ( nfirst2 < 1 )			/*right-hand sp2 is completely blank*/
  { nsmash = blanksignal;		/* signal intentional blanks */
    goto end_of_job; }			/* don't smash intentional blanks */
/* --- now check if preceding image in sp1 was an intentional blank --- */
if ( sp1->type == blanksignal )		/*blanksignal was propagated to us*/
  goto end_of_job;			/* don't smash intentional blank */
/* --- set firstcol1[] indicating right edge of sp1 --- */
for ( irow1=top1; irow1<=bot1; irow1++ ) /* for each row inside sp1 */
  for ( icol=width1-1; icol>=0; icol-- ) /* find last non-empty col in row */
    if ( getpixel(sp1->image,irow1-top1,icol) != 0 ) /* found a set pixel */
      {	firstcol1[irow1] = (width1-1)-icol; /* save #cols from right edge */
	nfirst1++;			/* bump #rows containing set pixels*/
	break; }			/* and go on to next row */
if ( nfirst1 < 1 )			/*left-hand sp1 is completely blank*/
  goto end_of_job;			/* don't smash intentional blanks */
/* -------------------------------------------------------------------------
find minimum separation
-------------------------------------------------------------------------- */
for ( irow2=top2; irow2<=bot2; irow2++ ) { /* check each row inside sp2 */
 int margin1, margin2=firstcol2[irow2];	/* #cols to first set pixel */
 if ( margin2 != blanksignal ) {	/* irow2 not an empty/blank row */
  for ( irow1=max2(irow2-smin,top1); ; irow1++ )
   if ( irow1 > min2(irow2+smin,bot1) ) break; /* upper bound check */
   else
    if ( (margin1=firstcol1[irow1]) != blanksignal ) { /*have non-blank row*/
     int dx=(margin1+margin2), dy=absval(irow2-irow1), ds=dx+dy; /* deltas */
     if ( ds >= smin ) continue;	/* min unchanged */
     if ( dy>smashmargin && dx<xmin && smin<9999 ) continue; /* dy alone */
     smin=ds; xmin=dx; ymin=dy;		/* set new min */
     } /* --- end-of-if(margin1!=blanksignal) --- */
  } /* --- end-of-if(margin2!=blanksignal) --- */
 if ( smin<2 ) goto end_of_job;		/* can't smash */
 } /* --- end-of-for(irow2) --- */
/*nsmash = min2(xmin,width2);*/		/* permissible smash */
nsmash = xmin;				/* permissible smash */
/* -------------------------------------------------------------------------
Back to caller with #pixels to smash sp1||sp2
-------------------------------------------------------------------------- */
end_of_job:
  /* --- debugging output --- */
  if ( msgfp!=NULL && msglevel >= 99 )	/* display for debugging */
    { fprintf(msgfp,"rastsmash> nsmash=%d, smashmargin=%d\n",
      nsmash,smashmargin);
      if ( msglevel >= 999 )		/* also display rasters */
	{ fprintf(msgfp,"rastsmash>left-hand image...\n");
	  if(sp1!=NULL) type_raster(sp1->image,msgfp); /* left image */
	  fprintf(msgfp,"rastsmash>right-hand image...\n");
	  if(sp2!=NULL) type_raster(sp2->image,msgfp); } /* right image */
      fflush(msgfp); }
  return ( nsmash );			/* back with #smash pixels */
} /* --- end-of-function rastsmash() --- */


/* ==========================================================================
 * Function:	rastsmashcheck ( term )
 * Purpose:	Check an exponent term to see if its leading symbol
 *		would make smashing dangerous
 * --------------------------------------------------------------------------
 * Arguments:	term (I)	char *  to null-terminated string
 *				containing right-hand exponent term about to
 *				be smashed against existing left-hand.
 * --------------------------------------------------------------------------
 * Returns:	( int )		1 if it's okay to smash term, or
 *				0 if smash is dangerous.
 * --------------------------------------------------------------------------
 * Notes:     o
 * ======================================================================= */
/* --- entry point --- */
int	rastsmashcheck ( char *term )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
int	isokay = 0;		/* 1 to signal okay to caller */
static	char nosmashchars[64] = "-.,="; /* don't smash these leading chars */
static	char *nosmashstrs[64] = { "\\frac", NULL }; /* or leading strings */
static	char *grayspace[64] = { "\\tiny", "\\small", "\\normalsize",
	"\\large", "\\Large", "\\LARGE", "\\huge", "\\Huge", NULL };
char	*expression = term;	/* local ptr to beginning of expression */
char	*token = NULL;  int i;	/* token = nosmashstrs[i] or grayspace[i] */
/* -------------------------------------------------------------------------
see if smash check enabled
-------------------------------------------------------------------------- */
if ( smashcheck < 1 ) {		/* no smash checking wanted */
  if ( smashcheck >= 0 )	/* -1 means check should always fail */
    isokay = 1;			/* otherwise (if 0), signal okay to smash */
  goto end_of_job; }		/* return to caller */
/* -------------------------------------------------------------------------
skip leading white and gray space
-------------------------------------------------------------------------- */
/* --- first check input --- */
if ( isempty(term) ) goto end_of_job; /* no input so return 0 to caller */
/* --- skip leading white space --- */
skipwhite(term);		/* skip leading white space */
if ( *term == '\000' ) goto end_of_job; /* nothing but white space */
/* --- skip leading gray space --- */
skipgray:
 for ( i=0; (token=grayspace[i]) != NULL; i++ ) /* check each grayspace */
  if ( strncmp(term,token,strlen(token)) == 0 ) { /* found grayspace */
   term += strlen(token);	/* skip past this grayspace token */
   skipwhite(term);		/* and skip any subsequent white space */
   if ( *term == '\000' ) {	/* nothing left so quit */
     if ( msgfp!=NULL && msglevel >= 99 ) /* display for debugging */
       fprintf(msgfp,"rastsmashcheck> only grayspace in %.32s\n",expression);
     goto end_of_job; }
   goto skipgray; }		/* restart grayspace check from beginning */
/* -------------------------------------------------------------------------
check for leading no-smash single char
-------------------------------------------------------------------------- */
/* --- don't smash if term begins with a "nosmash" char --- */
if ( (token=strchr(nosmashchars,*term)) != NULL ) {
  if ( msgfp!=NULL && msglevel >= 99 )	/* display for debugging */
    fprintf(msgfp,"rastsmashcheck> char %.1s found in %.32s\n",token,term);
  goto end_of_job; }
/* -------------------------------------------------------------------------
check for leading no-smash token
-------------------------------------------------------------------------- */
for ( i=0; (token=nosmashstrs[i]) != NULL; i++ ) /* check each nosmashstr */
 if ( strncmp(term,token,strlen(token)) == 0 ) { /* found a nosmashstr */
  if ( msgfp!=NULL && msglevel >= 99 )	/* display for debugging */
    fprintf(msgfp,"rastsmashcheck> token %s found in %.32s\n",token,term);
  goto end_of_job; }		/* so don't smash term */
/* -------------------------------------------------------------------------
back to caller
-------------------------------------------------------------------------- */
isokay = 1;			/* no problem, so signal okay to smash */
end_of_job:
  if ( msgfp!=NULL && msglevel >= 999 )	/* display for debugging */
    fprintf(msgfp,"rastsmashcheck> returning isokay=%d for \"%.32s\"\n",
    isokay,(expression==NULL?"<no input>":expression));
  return ( isokay );		/* back to caller with 1 if okay to smash */
} /* --- end-of-function rastsmashcheck() --- */


/* ==========================================================================
 * Function:	accent_subraster ( accent, width, height, direction, pixsz )
 * Purpose:	Allocate a new subraster of width x height
 *		(or maybe different dimensions, depending on accent),
 *		and draw an accent (\hat or \vec or \etc) that fills it
 * --------------------------------------------------------------------------
 * Arguments:	accent (I)	int containing either HATACCENT or VECACCENT,
 *				etc, indicating the type of accent desired
 *		width (I)	int containing desired width of accent (#cols)
 *		height (I)	int containing desired height of accent(#rows)
 *		direction (I)	int containing desired direction of accent,
 *				+1=right, -1=left, 0=left/right
 *		pixsz (I)	int containing 1 for bitmap, 8 for bytemap
 * --------------------------------------------------------------------------
 * Returns:	( subraster * )	ptr to newly-allocated subraster with accent,
 *				or NULL for any error.
 * --------------------------------------------------------------------------
 * Notes:     o	Some accents have internally-determined dimensions,
 *		and caller should check dimensions in returned subraster
 * ======================================================================= */
/* --- entry point --- */
subraster *accent_subraster (  int accent, int width, int height,
int direction, int pixsz )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
/* --- general info --- */
raster	*new_raster(), *rp=NULL;	/*raster containing desired accent*/
subraster *new_subraster(), *sp=NULL;	/* subraster returning accent */
int	delete_raster(), delete_subraster(); /*free allocated raster on err*/
int	line_raster(),			/* draws lines */
	rule_raster(),			/* draw solid boxes */
	thickness = 1;			/* line thickness */
/*int	pixval = (pixsz==1? 1 : (pixsz==8?255:(-1)));*/ /*black pixel value*/
/* --- other working info --- */
int	col0, col1,			/* cols for line */
	row0, row1;			/* rows for line */
subraster *get_delim(), *accsp=NULL;	/*find suitable cmex10 symbol/accent*/
/* --- info for under/overbraces, tildes, etc --- */
char	brace[16];			/*"{" for over, "}" for under, etc*/
raster	*rastrot(),			/* rotate { for overbrace, etc */
	*rastcpy();			/* may need copy of original */
subraster *arrow_subraster();		/* rightarrow for vec */
subraster *rastack();			/* stack accent atop extra space */
int	iswidthneg = 0;			/* set true if width<0 arg passed */
int	serifwidth=0;			/* serif for surd */
int	isBig=0;			/* true for ==>arrow, false for -->*/
/* -------------------------------------------------------------------------
initialization
-------------------------------------------------------------------------- */
if ( width < 0 ) { width=(-width); iswidthneg=1; } /* set neg width flag */
/* -------------------------------------------------------------------------
outer switch() traps accents that may change caller's height,width
-------------------------------------------------------------------------- */
switch ( accent )
 {
 default:
  /* -----------------------------------------------------------------------
  inner switch() first allocates fixed-size raster for accents that don't
  ------------------------------------------------------------------------ */
  if ( (rp = new_raster(width,height,pixsz)) /* allocate fixed-size raster */
  !=   NULL )				/* and if we succeeded... */
   switch ( accent )			/* ...draw requested accent in it */
    {
    /* --- unrecognized request --- */
    default: delete_raster(rp);		/* unrecognized accent requested */
	rp = NULL;  break;		/* so free raster and signal error */
    /* --- bar request --- */
    case UNDERBARACCENT:
    case BARACCENT:
	thickness = 1; /*height-1;*/	/* adjust thickness */
	if ( accent == BARACCENT )	/* bar is above expression */
	 { row0 = row1 = max2(height-3,0); /* row numbers for overbar */
	   line_raster(rp,row0,0,row1,width-1,thickness); } /*blanks at bot*/
	else				/* underbar is below expression */
	 { row0 = row1 = min2(2,height-1); /* row numbers for underbar */
	   line_raster(rp,row0,0,row1,width-1,thickness); } /*blanks at top*/
	break;
    /* --- dot request --- */
    case DOTACCENT:
	thickness = height-1;		/* adjust thickness */
	/*line_raster(rp,0,width/2,1,(width/2)+1,thickness);*//*centered dot*/
	rule_raster(rp,0,(width+1-thickness)/2,thickness,thickness,3); /*box*/
	break;
    /* --- ddot request --- */
    case DDOTACCENT:
	thickness = height-1;		/* adjust thickness */
	col0 = max2((width+1)/3-(thickness/2)-1,0); /* one-third of width */
	col1 = min2((2*width+1)/3-(thickness/2)+1,width-thickness); /*2/3rds*/
	if ( col0+thickness >= col1 )	/* dots overlap */
	  { col0 = max2(col0-1,0);	/* try moving left dot more left */
	    col1 = min2(col1+1,width-thickness); } /* and right dot right */
	if ( col0+thickness >= col1 )	/* dots _still_ overlap */
	  thickness = max2(thickness-1,1); /* so try reducing thickness */
	/*line_raster(rp,0,col0,1,col0+1,thickness);*//*set dot at 1st third*/
	/*line_raster(rp,0,col1,1,col1+1,thickness);*//*and another at 2nd*/
	rule_raster(rp,0,col0,thickness,thickness,3); /*box at 1st third*/
	rule_raster(rp,0,col1,thickness,thickness,3); /*box at 2nd third*/
	break;
    /* --- hat request --- */
    case HATACCENT:
	thickness = 1; /*(width<=12? 2 : 3);*/	/* adjust thickness */
	line_raster(rp,height-1,0,0,width/2,thickness);    /* / part of hat*/
	line_raster(rp,0,(width-1)/2,height-1,width-1,thickness); /* \ part*/
	break;
    /* --- sqrt request --- */
    case SQRTACCENT:
	serifwidth = SURDSERIFWIDTH(height); /* leading serif on surd */
	col1 = SQRTWIDTH(height,(iswidthneg?1:2)) - 1; /*right col of sqrt*/
	/*col0 = (col1-serifwidth+2)/3;*/ /* midpoint col of sqrt */
	col0 = (col1-serifwidth+1)/2;	/* midpoint col of sqrt */
	row0 = max2(1,((height+1)/2)-2); /* midpoint row of sqrt */
	row1 = height-1;		/* bottom row of sqrt */
	/*line_raster(rp,row0,0,row1,col0,thickness);*/ /*descending portion*/
	line_raster(rp,row0+serifwidth,0,row0,serifwidth,thickness);
	line_raster(rp,row0,serifwidth,row1,col0,thickness); /* descending */
	line_raster(rp,row1,col0,0,col1,thickness); /* ascending portion */
	line_raster(rp,0,col1,0,width-1,thickness); /*overbar of thickness 1*/
	break;
    } /* --- end-of-inner-switch(accent) --- */
    break;				/* break from outer accent switch */
 /* --- underbrace, overbrace request --- */
 case UNDERBRACE:
 case OVERBRACE:
    if ( accent == UNDERBRACE ) strcpy(brace,"}"); /* start with } brace */
    if ( accent ==  OVERBRACE ) strcpy(brace,"{"); /* start with { brace */
    if ( (accsp=get_delim(brace,width,CMEX10)) /* use width for height */
    !=  NULL )				/* found desired brace */
      { rp = rastrot(accsp->image);	/* rotate 90 degrees clockwise */
	delete_subraster(accsp); }	/* and free subraster "envelope" */
    break;
 /* --- hat request --- */
 case HATACCENT:
    if ( accent == HATACCENT ) strcpy(brace,"<"); /* start with < */
    if ( (accsp=get_delim(brace,width,CMEX10)) /* use width for height */
    !=  NULL )				/* found desired brace */
      { rp = rastrot(accsp->image);	/* rotate 90 degrees clockwise */
	delete_subraster(accsp); }	/* and free subraster "envelope" */
    break;
 /* --- vec request --- */
 case VECACCENT:
    height = 2*(height/2) + 1;		/* force height odd */
    if ( absval(direction) >= 9 ) {	/* want ==> arrow rather than --> */
      isBig = 1;			/* signal "Big" arrow */
      direction -= 10; }		/* reset direction = +1, -1, or 0 */
    if ((accsp=arrow_subraster(width,height,pixsz,direction,isBig)) /*arrow*/
    !=  NULL )				/* succeeded */
	{ rp = accsp->image;		/* "extract" raster with bitmap */
	  free((void *)accsp); }	/* and free subraster "envelope" */
    break;
 /* --- tilde request --- */
 case TILDEACCENT:
    accsp=(width<25? get_delim("\\sim",-width,CMSY10) :
		     get_delim("~",-width,CMEX10)); /*width search for tilde*/
    if ( accsp !=  NULL )		/* found desired tilde */
      if ( (sp=rastack(new_subraster(1,1,pixsz),accsp,1,0,1,3))/*space below*/
      !=  NULL )			/* have tilde with space below it */
	{ rp = sp->image;		/* "extract" raster with bitmap */
	  free((void *)sp);		/* and free subraster "envelope" */
	  leftsymdef = NULL; }		/* so \tilde{x}^2 works properly */
    break;
 } /* --- end-of-outer-switch(accent) --- */
/* -------------------------------------------------------------------------
if we constructed accent raster okay, embed it in a subraster and return it
-------------------------------------------------------------------------- */
/* --- if all okay, allocate subraster to contain constructed raster --- */
if ( rp != NULL ) {			/* accent raster constructed okay */
  if ( (sp=new_subraster(0,0,0))	/* allocate subraster "envelope" */
  ==   NULL )				/* and if we fail to allocate */
    delete_raster(rp);			/* free now-unneeded raster */
  else					/* subraster allocated okay */
    { /* --- init subraster parameters, embedding raster in it --- */
      sp->type = IMAGERASTER;		/* constructed image */
      sp->image = rp;			/* raster we just constructed */
      sp->size = (-1);			/* can't set font size here */
      sp->baseline = 0; }		/* can't set baseline here */
  } /* --- end-of-if(rp!=NULL) --- */
/* --- return subraster containing desired accent to caller --- */
return ( sp );				/* return accent or NULL to caller */
} /* --- end-of-function accent_subraster() --- */


/* ==========================================================================
 * Function:	arrow_subraster ( width, height, pixsz, drctn, isBig )
 * Purpose:	Allocate a raster/subraster and draw left/right arrow in it
 * --------------------------------------------------------------------------
 * Arguments:	width (I)	int containing number of cols for arrow
 *		height (I)	int containing number of rows for arrow
 *		pixsz (I)	int containing 1 for bitmap, 8 for bytemap
 *		drctn (I)	int containing +1 for right arrow,
 *				or -1 for left, 0 for leftright
 *		isBig (I)	int containing 1/true for \Long arrows,
 *				or false for \long arrows, i.e.,
 *				true for ===> or false for --->.
 * --------------------------------------------------------------------------
 * Returns:	( subraster * )	ptr to constructed left/right arrow
 *				or NULL for any error.
 * --------------------------------------------------------------------------
 * Notes:     o
 * ======================================================================= */
/* --- entry point --- */
subraster *arrow_subraster ( int width, int height, int pixsz,
				int drctn, int isBig )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
subraster *new_subraster(), *arrowsp=NULL; /* allocate arrow subraster */
int	rule_raster();			/* draw arrow line */
int	irow, midrow=height/2;		/* index, midrow is arrowhead apex */
int	icol, thickness=(height>15?2:2); /* arrowhead thickness and index */
int	pixval = (pixsz==1? 1 : (pixsz==8?255:(-1))); /* black pixel value */
int	ipix,				/* raster pixmap[] index */
	npix = width*height;		/* #pixels malloced in pixmap[] */
/* -------------------------------------------------------------------------
allocate raster/subraster and draw arrow line
-------------------------------------------------------------------------- */
if ( height < 3 ) { height=3; midrow=1; }	/* set minimum height */
if ( (arrowsp=new_subraster(width,height,pixsz)) /* allocate empty raster */
==   NULL ) goto end_of_job;			/* and quit if failed */
if ( !isBig )					/* single line */
  rule_raster(arrowsp->image,midrow,0,width,1,0); /*draw line across midrow*/
else
  { int	delta = (width>6? (height>15? 3: (height>7? 2 : 1)) : 1);
    rule_raster(arrowsp->image,midrow-delta,delta,width-2*delta,1,0);
    rule_raster(arrowsp->image,midrow+delta,delta,width-2*delta,1,0); }
/* -------------------------------------------------------------------------
construct arrowhead(s)
-------------------------------------------------------------------------- */
for ( irow=0; irow<height; irow++ )		/* for each row of arrow */
  {
  int	delta = abs(irow-midrow);		/*arrowhead offset for irow*/
  /* --- right arrowhead --- */
  if ( drctn >= 0 )				/* right arrowhead wanted */
    for ( icol=0; icol<thickness; icol++ )	/* for arrowhead thickness */
     { ipix = ((irow+1)*width - 1) - delta - icol; /* rightmost-delta-icol */
       if ( ipix >= 0 ) {				/* bounds check */
	if ( pixsz == 1 )			/* have a bitmap */
	  setlongbit((arrowsp->image)->pixmap,ipix);/*turn on arrowhead bit*/
	else					/* should have a bytemap */
	 if ( pixsz == 8 )			/* check pixsz for bytemap */
	  ((arrowsp->image)->pixmap)[ipix] = pixval; } }/*set arrowhead byte*/
  /* --- left arrowhead (same as right except for ipix calculation) --- */
  if ( drctn <= 0 )				/* left arrowhead wanted */
    for ( icol=0; icol<thickness; icol++ )	/* for arrowhead thickness */
     { ipix = irow*width + delta + icol;	/* leftmost bit+delta+icol */
       if ( ipix < npix ) {			/* bounds check */
	if ( pixsz == 1 )			/* have a bitmap */
	  setlongbit((arrowsp->image)->pixmap,ipix);/*turn on arrowhead bit*/
	else					/* should have a bytemap */
	 if ( pixsz == 8 )			/* check pixsz for bytemap */
	  ((arrowsp->image)->pixmap)[ipix] = pixval; } }/*set arrowhead byte*/
  } /* --- end-of-for(irow) --- */
end_of_job:
  return ( arrowsp );			/*back to caller with arrow or NULL*/
} /* --- end-of-function arrow_subraster() --- */


/* ==========================================================================
 * Function:	uparrow_subraster ( width, height, pixsz, drctn, isBig )
 * Purpose:	Allocate a raster/subraster and draw up/down arrow in it
 * --------------------------------------------------------------------------
 * Arguments:	width (I)	int containing number of cols for arrow
 *		height (I)	int containing number of rows for arrow
 *		pixsz (I)	int containing 1 for bitmap, 8 for bytemap
 *		drctn (I)	int containing +1 for up arrow,
 *				or -1 for down, or 0 for updown
 *		isBig (I)	int containing 1/true for \Long arrows,
 *				or false for \long arrows, i.e.,
 *				true for ===> or false for --->.
 * --------------------------------------------------------------------------
 * Returns:	( subraster * )	ptr to constructed up/down arrow
 *				or NULL for any error.
 * --------------------------------------------------------------------------
 * Notes:     o
 * ======================================================================= */
/* --- entry point --- */
subraster *uparrow_subraster ( int width, int height, int pixsz,
					int drctn, int isBig )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
subraster *new_subraster(), *arrowsp=NULL; /* allocate arrow subraster */
int	rule_raster();			/* draw arrow line */
int	icol, midcol=width/2;		/* index, midcol is arrowhead apex */
int	irow, thickness=(width>15?2:2);	/* arrowhead thickness and index */
int	pixval = (pixsz==1? 1 : (pixsz==8?255:(-1))); /* black pixel value */
int	ipix,				/* raster pixmap[] index */
	npix = width*height;		/* #pixels malloced in pixmap[] */
/* -------------------------------------------------------------------------
allocate raster/subraster and draw arrow line
-------------------------------------------------------------------------- */
if ( width < 3 ) { width=3; midcol=1; }		/* set minimum width */
if ( (arrowsp=new_subraster(width,height,pixsz)) /* allocate empty raster */
==   NULL ) goto end_of_job;			/* and quit if failed */
if ( !isBig )					/* single line */
  rule_raster(arrowsp->image,0,midcol,1,height,0); /*draw line down midcol*/
else
  { int	delta = (height>6? (width>15? 3: (width>7? 2 : 1)) : 1);
    rule_raster(arrowsp->image,delta,midcol-delta,1,height-2*delta,0);
    rule_raster(arrowsp->image,delta,midcol+delta,1,height-2*delta,0); }
/* -------------------------------------------------------------------------
construct arrowhead(s)
-------------------------------------------------------------------------- */
for ( icol=0; icol<width; icol++ )		/* for each col of arrow */
  {
  int	delta = abs(icol-midcol);		/*arrowhead offset for icol*/
  /* --- up arrowhead --- */
  if ( drctn >= 0 )				/* up arrowhead wanted */
    for ( irow=0; irow<thickness; irow++ )	/* for arrowhead thickness */
     { ipix = (irow+delta)*width + icol;	/* leftmost+icol */
       if ( ipix < npix ) {			/* bounds check */
	if ( pixsz == 1 )			/* have a bitmap */
	  setlongbit((arrowsp->image)->pixmap,ipix);/*turn on arrowhead bit*/
	else					/* should have a bytemap */
	 if ( pixsz == 8 )			/* check pixsz for bytemap */
	  ((arrowsp->image)->pixmap)[ipix] = pixval; } }/*set arrowhead byte*/
  /* --- down arrowhead (same as up except for ipix calculation) --- */
  if ( drctn <= 0 )				/* down arrowhead wanted */
    for ( irow=0; irow<thickness; irow++ )	/* for arrowhead thickness */
     { ipix = (height-1-delta-irow)*width + icol; /* leftmost + icol */
       if ( ipix > 0 ) {			/* bounds check */
	if ( pixsz == 1 )			/* have a bitmap */
	  setlongbit((arrowsp->image)->pixmap,ipix);/*turn on arrowhead bit*/
	else					/* should have a bytemap */
	 if ( pixsz == 8 )			/* check pixsz for bytemap */
	  ((arrowsp->image)->pixmap)[ipix] = pixval; } }/*set arrowhead byte*/
  } /* --- end-of-for(icol) --- */
end_of_job:
  return ( arrowsp );			/*back to caller with arrow or NULL*/
} /* --- end-of-function uparrow_subraster() --- */


/* ==========================================================================
 * Function:	rule_raster ( rp, top, left, width, height, type )
 * Purpose:	Draw a solid or dashed line (or box) in existing raster rp,
 *		starting at top,left with dimensions width,height.
 * --------------------------------------------------------------------------
 * Arguments:	rp (I)		raster *  to raster in which rule
 *				will be drawn
 *		top (I)		int containing row at which top-left corner
 *				of rule starts (0 is topmost)
 *		left (I)	int containing col at which top-left corner
 *				of rule starts (0 is leftmost)
 *		width (I)	int containing number of cols for rule
 *		height (I)	int containing number of rows for rule
 *		type (I)	int containing 0 for solid rule,
 *				1 for horizontal dashes, 2 for vertical
 *				3 for solid rule with corners removed (bevel)
 *				4 for strut (nothing drawn)
 * --------------------------------------------------------------------------
 * Returns:	( int )		1 if rule drawn okay,
 *				or 0 for any error.
 * --------------------------------------------------------------------------
 * Notes:     o	Rule line is implicitly "horizontal" or "vertical" depending
 *		on relative width,height dimensions.  It's a box if they're
 *		more or less comparable.
 * ======================================================================= */
/* --- entry point --- */
int	rule_raster ( raster *rp, int top, int left,
		int width, int height, int type )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
int	irow=0, icol=0;		/* indexes over rp raster */
int	ipix = 0,		/* raster pixmap[] index */
	npix = rp->width * rp->height; /* #pixels malloced in rp->pixmap[] */
int	isfatal = 0;		/* true to abend on out-of-bounds error */
int	hdash=1, vdash=2,	/* type for horizontal, vertical dashes */
	bevel=99/*3*/, strut=4;	/* type for bevel (turned off), strut */
int	dashlen=3, spacelen=2,	/* #pixels for dash followed by space */
	isdraw=1;		/* true when drawing dash (init for solid) */
/* -------------------------------------------------------------------------
Check args
-------------------------------------------------------------------------- */
if ( rp == (raster *)NULL ) {	/* no raster arg supplied */
  if ( workingbox != (subraster *)NULL )  /* see if we have a workingbox */
    rp = workingbox->image;	/* use workingbox if possible */
  else return ( 0 ); }		/* otherwise signal error to caller */
if ( type == bevel )		/* remove corners of solid box */
  if ( width<3 || height<3 ) type=0; /* too small to remove corners */
/* -------------------------------------------------------------------------
Fill line/box
-------------------------------------------------------------------------- */
if ( width > 0 )				/* zero width implies strut*/
 for ( irow=top; irow<top+height; irow++ )	/* for each scan line */
  {
  if ( type == strut ) isdraw = 0;		/* draw nothing for strut */
  if ( type == vdash )				/*set isdraw for vert dash*/
    isdraw = (((irow-top)%(dashlen+spacelen)) < dashlen);
  ipix = irow*rp->width + left - 1;		/*first pixel preceding icol*/
  for ( icol=left; icol<left+width; icol++ )	/* each pixel in scan line */
    {
    if ( type == bevel ) {			/* remove corners of box */
      if ( (irow==top && icol==left)		/* top-left corner */
      ||   (irow==top && icol>=left+width-1)	/* top-right corner */
      ||   (irow>=top+height-1 && icol==left)	/* bottom-left corner */
      ||   (irow>=top+height-1 && icol>=left+width-1) ) /* bottom-right */
	isdraw = 0;  else isdraw = 1; }		/*set isdraw to skip corner*/
    if ( type == hdash )			/*set isdraw for horiz dash*/
      isdraw = (((icol-left)%(dashlen+spacelen)) < dashlen);
    if ( ++ipix >= npix )			/* bounds check failed */
         if ( isfatal ) goto end_of_job;	/* abort if error is fatal */
         else break;				/*or just go on to next row*/
    else					/*ibit is within rp bounds*/
      if ( isdraw ) {				/*and we're drawing this bit*/
	if ( rp->pixsz == 1 )			/* have a bitmap */
	  setlongbit(rp->pixmap,ipix);		/* so turn on bit in line */
	else					/* should have a bytemap */
	 if ( rp->pixsz == 8 )			/* check pixsz for bytemap */
	  ((unsigned char *)(rp->pixmap))[ipix] = 255; } /* set black byte */
    } /* --- end-of-for(icol) --- */
  } /* --- end-of-for(irow) --- */
end_of_job:
  return ( isfatal? (ipix<npix? 1:0) : 1 );
} /* --- end-of-function rule_raster() --- */


/* ==========================================================================
 * Function:	line_raster ( rp,  row0, col0,  row1, col1,  thickness )
 * Purpose:	Draw a line from row0,col0 to row1,col1 of thickness
 *		in existing raster rp.
 * --------------------------------------------------------------------------
 * Arguments:	rp (I)		raster *  to raster in which a line
 *				will be drawn
 *		row0 (I)	int containing row at which
 *				line will start (0 is topmost)
 *		col0 (I)	int containing col at which
 *				line will start (0 is leftmost)
 *		row1 (I)	int containing row at which
 *				line will end (rp->height-1 is bottom-most)
 *		col1 (I)	int containing col at which
 *				line will end (rp->width-1 is rightmost)
 *		thickness (I)	int containing number of pixels/bits
 *				thick the line will be
 * --------------------------------------------------------------------------
 * Returns:	( int )		1 if line drawn okay,
 *				or 0 for any error.
 * --------------------------------------------------------------------------
 * Notes:     o	if row0==row1, a horizontal line is drawn
 *		between col0 and col1, with row0(==row1) the top row
 *		and row0+(thickness-1) the bottom row
 *	      o	if col0==col1, a vertical bar is drawn
 *		between row0 and row1, with col0(==col1) the left col
 *		and col0+(thickness-1) the right col
 *	      o	if both the above, you get a square thickness x thickness
 *		whose top-left corner is row0,col0.
 * ======================================================================= */
/* --- entry point --- */
int	line_raster ( raster *rp, int row0, int col0,
	int row1, int col1, int thickness )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
int	irow=0, icol=0,		/* indexes over rp raster */
	locol=col0, hicol=col1,	/* col limits at irow */
	lorow=row0, hirow=row1;	/* row limits at icol */
int	width=rp->width, height=rp->height; /* dimensions of input raster */
int	ipix = 0,		/* raster pixmap[] index */
	npix = width*height;	/* #pixels malloced in rp->pixmap[] */
int	isfatal = 0;		/* true to abend on out-of-bounds error */
int	isline=(row1==row0), isbar=(col1==col0); /*true if slope a=0,\infty*/
double	dy = row1-row0 /* + (row1>=row0? +1.0 : -1.0) */, /* delta-x */
	dx = col1-col0 /* + (col1>=col0? +1.0 : -1.0) */, /* delta-y */
	a= (isbar||isline? 0.0 : dy/dx), /* slope = tan(theta) = dy/dx */
	xcol=0, xrow=0;		/* calculated col at irow, or row at icol */
double	ar = ASPECTRATIO,	/* aspect ratio width/height of one pixel */
	xwidth= (isline? 0.0 :	/*#pixels per row to get sloped line thcknss*/
		((double)thickness)*sqrt((dx*dx)+(dy*dy*ar*ar))/fabs(dy*ar)),
	xheight = 1.0;
int	line_recurse(), isrecurse=1; /* true to draw line recursively */
/* -------------------------------------------------------------------------
Check args
-------------------------------------------------------------------------- */
if ( rp == (raster *)NULL ) {	/* no raster arg supplied */
  if ( workingbox != (subraster *)NULL )  /* see if we have a workingbox */
    rp = workingbox->image;	/* use workingbox if possible */
  else return ( 0 ); }		/* otherwise signal error to caller */
/* -------------------------------------------------------------------------
Initialization
-------------------------------------------------------------------------- */
if ( msgfp!=NULL && msglevel>=29 ) {		/* debugging */
   fprintf(msgfp,"line_raster> row,col0=%d,%d row,col1=%d,%d, thickness=%d\n"
   "\t dy,dx=%3.1f,%3.1f, a=%4.3f, xwidth=%4.3f\n",
   row0,col0, row1,col1, thickness,  dy,dx, a, xwidth); fflush(msgfp); }
/* --- check for recursive line drawing --- */
if ( isrecurse ) {		/* drawing lines recursively */
 for ( irow=0; irow<thickness; irow++ )		/* each line 1 pixel thick */
  { double xrow0=(double)row0, xcol0=(double)col0,
	xrow1=(double)row1, xcol1=(double)col1;
    if ( isline ) xrow0 = xrow1 = (double)(row0+irow);
    else if ( isbar ) xcol0 = xcol1 = (double)(col0+irow);
    if( xrow0>(-0.001) && xcol0>(-0.001)	/*check line inside raster*/
    &&  xrow1<((double)(height-1)+0.001) && xcol1<((double)(width-1)+0.001) )
      line_recurse(rp,xrow0,xcol0,xrow1,xcol1,thickness); }
 return ( 1 ); }
/* --- set params for horizontal line or vertical bar --- */
if ( isline )					/*interpret row as top row*/
  row1 = row0 + (thickness-1);			/* set bottom row for line */
if ( 0&&isbar )					/*interpret col as left col*/
  hicol = col0 + (thickness-1);			/* set right col for bar */
/* -------------------------------------------------------------------------
draw line one row at a time
-------------------------------------------------------------------------- */
for ( irow=min2(row0,row1); irow<=max2(row0,row1); irow++ ) /*each scan line*/
  {
  if ( !isbar && !isline )			/* neither vert nor horiz */
    { xcol  = col0 + ((double)(irow-row0))/a;	/* "middle" col in irow */
      locol = max2((int)(xcol-0.5*(xwidth-1.0)),0); /* leftmost col */
      hicol = min2((int)(xcol+0.5*(xwidth-0.0)),max2(col0,col1)); } /*right*/
  if ( msgfp!=NULL && msglevel>=29 )		/* debugging */
    fprintf(msgfp,"\t irow=%d, xcol=%4.2f, lo,hicol=%d,%d\n",
    irow,xcol,locol,hicol);
  ipix = irow*rp->width + min2(locol,hicol) - 1; /*first pix preceding icol*/
  for ( icol=min2(locol,hicol); icol<=max2(locol,hicol); icol++ ) /*each pix*/
    if ( ++ipix >= npix )			/* bounds check failed */
	if ( isfatal ) goto end_of_job;	/* abort if error is fatal */
	else break;				/*or just go on to next row*/
    else					/* turn on pixel in line */
	if ( rp->pixsz == 1 )			/* have a pixel bitmap */
	  setlongbit(rp->pixmap,ipix);		/* so turn on bit in line */
	else					/* should have a bytemap */
	 if ( rp->pixsz == 8 )			/* check pixsz for bytemap */
	  ((unsigned char *)(rp->pixmap))[ipix] = 255; /* set black byte */
  } /* --- end-of-for(irow) --- */
/* -------------------------------------------------------------------------
now _redraw_ line one col at a time to avoid "gaps"
-------------------------------------------------------------------------- */
if ( 1 )
 for ( icol=min2(col0,col1); icol<=max2(col0,col1); icol++ )/*each scan line*/
  {
  if ( !isbar && !isline )			/* neither vert nor horiz */
    { xrow  = row0 + ((double)(icol-col0))*a;	/* "middle" row in icol */
      lorow = max2((int)(xrow-0.5*(xheight-1.0)),0); /* topmost row */
      hirow = min2((int)(xrow+0.5*(xheight-0.0)),max2(row0,row1)); } /*bot*/
  if ( msgfp!=NULL && msglevel>=29 )		/* debugging */
    fprintf(msgfp,"\t icol=%d, xrow=%4.2f, lo,hirow=%d,%d\n",
    icol,xrow,lorow,hirow);
  ipix = irow*rp->width + min2(locol,hicol) - 1; /*first pix preceding icol*/
  for ( irow=min2(lorow,hirow); irow<=max2(lorow,hirow); irow++ ) /*each pix*/
    if ( irow<0 || irow>=rp->height
    ||   icol<0 || icol>=rp->width )		/* bounds check */
      if ( isfatal ) goto end_of_job;		/* abort if error is fatal */
      else continue;				/*or just go on to next row*/
    else
      setpixel(rp,irow,icol,255);		/* set pixel at irow,icol */
  } /* --- end-of-for(irow) --- */
/* -------------------------------------------------------------------------
Back to caller with 1=okay, 0=failed.
-------------------------------------------------------------------------- */
end_of_job:
  return ( isfatal? (ipix<npix? 1:0) : 1 );
} /* --- end-of-function line_raster() --- */


/* ==========================================================================
 * Function:	line_recurse ( rp,  row0, col0,  row1, col1,  thickness )
 * Purpose:	Draw a line from row0,col0 to row1,col1 of thickness
 *		in existing raster rp.
 * --------------------------------------------------------------------------
 * Arguments:	rp (I)		raster *  to raster in which a line
 *				will be drawn
 *		row0 (I)	double containing row at which
 *				line will start (0 is topmost)
 *		col0 (I)	double containing col at which
 *				line will start (0 is leftmost)
 *		row1 (I)	double containing row at which
 *				line will end (rp->height-1 is bottom-most)
 *		col1 (I)	double containing col at which
 *				line will end (rp->width-1 is rightmost)
 *		thickness (I)	int containing number of pixels/bits
 *				thick the line will be
 * --------------------------------------------------------------------------
 * Returns:	( int )		1 if line drawn okay,
 *				or 0 for any error.
 * --------------------------------------------------------------------------
 * Notes:     o	Recurses, drawing left- and right-halves of line
 *		until a horizontal or vertical segment is found
 * ======================================================================= */
/* --- entry point --- */
int	line_recurse ( raster *rp, double row0, double col0,
	double row1, double col1, int thickness )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
double	delrow = fabs(row1-row0),	/* 0 if line horizontal */
	delcol = fabs(col1-col0),	/* 0 if line vertical */
	tolerance = 0.5;		/* draw line when it goes to point */
double	midrow = 0.5*(row0+row1),	/* midpoint row */
	midcol = 0.5*(col0+col1);	/* midpoint col */
/* -------------------------------------------------------------------------
recurse if either delta > tolerance
-------------------------------------------------------------------------- */
if ( delrow > tolerance			/* row hasn't converged */
||   delcol > tolerance )		/* col hasn't converged */
  { line_recurse(rp,row0,col0,midrow,midcol,thickness); /* left half */
    line_recurse(rp,midrow,midcol,row1,col1,thickness); /* right half */
    return ( 1 ); }
/* -------------------------------------------------------------------------
draw converged point
-------------------------------------------------------------------------- */
setpixel(rp,iround(midrow),iround(midcol),255); /*set pixel at midrow,midcol*/
return ( 1 );
} /* --- end-of-function line_recurse() --- */


/* ==========================================================================
 * Function:	circle_raster ( rp,  row0, col0,  row1, col1,
 *		thickness, quads )
 * Purpose:	Draw quad(rant)s of an ellipse in box determined by
 *		diagonally opposite corner points (row0,col0) and
 *		(row1,col1), of thickness pixels in existing raster rp.
 * --------------------------------------------------------------------------
 * Arguments:	rp (I)		raster *  to raster in which an ellipse
 *				will be drawn
 *		row0 (I)	int containing 1st corner row bounding ellipse
 *				(0 is topmost)
 *		col0 (I)	int containing 1st corner col bounding ellipse
 *				(0 is leftmost)
 *		row1 (I)	int containing 2nd corner row bounding ellipse
 *				(rp->height-1 is bottom-most)
 *		col1 (I)	int containing 2nd corner col bounding ellipse
 *				(rp->width-1 is rightmost)
 *		thickness (I)	int containing number of pixels/bits
 *				thick the ellipse arc line will be
 *		quads (I)	char * to null-terminated string containing
 *				any subset/combination of "1234" specifying
 *				which quadrant(s) of ellipse to draw.
 *				NULL ptr draws all four quadrants;
 *				otherwise 1=upper-right quadrant,
 *				2=uper-left, 3=lower-left, 4=lower-right,
 *				i.e., counterclockwise from 1=positive quad.
 * --------------------------------------------------------------------------
 * Returns:	( int )		1 if ellipse drawn okay,
 *				or 0 for any error.
 * --------------------------------------------------------------------------
 * Notes:     o	row0==row1 or col0==col1 are errors
 *	      o	using ellipse equation x^2/a^2 + y^2/b^2 = 1
 * ======================================================================= */
/* --- entry point --- */
int	circle_raster ( raster *rp, int row0, int col0,
	int row1, int col1, int thickness, char *quads )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
/* --- lower-left and upper-right bounding points (in our coords) --- */
int	lorow = min2(row0,row1),	/* lower bounding row (top of box) */
	locol = min2(col0,col1),	/* lower bounding col (left of box)*/
	hirow = max2(row0,row1),	/* upper bounding row (bot of box) */
	hicol = max2(col0,col1);	/* upper bounding col (right of box)*/
/* --- a and b ellipse params --- */
int	width = hicol-locol+1,		/* width of bounding box */
	height= hirow-lorow+1,		/* height of bounding box */
	islandscape = (width>=height? 1:0); /*true if ellipse lying on side*/
double	a = ((double)width)/2.0,	/* x=a when y=0 */
	b = ((double)height)/2.0,	/* y=b when x=0 */
	abmajor = (islandscape? a : b),	/* max2(a,b) */
	abminor = (islandscape? b : a),	/* min2(a,b) */
	abmajor2 = abmajor*abmajor,	/* abmajor^2 */
	abminor2 = abminor*abminor;	/* abminor^2 */
/* --- other stuff --- */
int	imajor=0, nmajor=max2(width,height), /*index, #pixels on major axis*/
	iminor=0, nminor=min2(width,height); /* solved index on minor axis */
int	irow, icol,			/* raster indexes at circumference */
	rsign=1, csign=1;		/* row,col signs, both +1 in quad 1*/
double	midrow= ((double)(row0+row1))/2.0, /* center row */
	midcol= ((double)(col0+col1))/2.0; /* center col */
double	xy, xy2,			/* major axis ellipse coord */
	yx2, yx;			/* solved minor ellipse coord */
int	isokay = 1;			/* true if no pixels out-of-bounds */
char	*qptr=NULL, *allquads="1234";	/* quadrants if input quads==NULL */
int	circle_recurse(), isrecurse=1;	/* true to draw ellipse recursively*/
/* -------------------------------------------------------------------------
pixel-by-pixel along positive major axis, quit when it goes negative
-------------------------------------------------------------------------- */
if ( quads == NULL ) quads = allquads;	/* draw all quads, or only user's */
if ( msgfp!=NULL && msglevel>=39 )	/* debugging */
  fprintf(msgfp,"circle_raster> width,height;quads=%d,%d,%s\n",
  width,height,quads);
if ( nmajor < 1 ) isokay = 0;		/* problem with input args */
else
 {
 if ( isrecurse )			/* use recursive algorithm */
  {
  for ( qptr=quads; *qptr!='\000'; qptr++ ) /* for each character in quads */
   {
   double theta0=0.0, theta1=0.0;	/* set thetas based on quadrant */
   switch ( *qptr )			/* check for quadrant 1,2,3,4 */
    { default:				/* unrecognized, assume quadrant 1 */
      case '1': theta0=  0.0; theta1= 90.0; break;   /* first quadrant */
      case '2': theta0= 90.0; theta1=180.0; break;   /* second quadrant */
      case '3': theta0=180.0; theta1=270.0; break;   /* third quadrant */
      case '4': theta0=270.0; theta1=360.0; break; } /* fourth quadrant */
   circle_recurse(rp,row0,col0,row1,col1,thickness,theta0,theta1);
   } /* --- end-of-for(qptr) --- */
  return ( 1 );
  } /* --- end-of-if(isrecurse) --- */
 for ( imajor=(nmajor+1)/2; ; imajor-- )
  {
  /* --- xy is coord along major axis, yx is "solved" along minor axis --- */
  xy  = ((double)imajor);		/* xy = abmajor ... 0 */
  if ( xy < 0.0 ) break;		/* negative side symmetrical */
  yx2 = abminor2*(1.0 - xy*xy/abmajor2); /* "solve" ellipse equation */
  yx  = (yx2>0.0? sqrt(yx2) : 0.0);	/* take sqrt if possible */
  iminor = iround(yx);			/* nearest integer */
  /* --- set pixels for each requested quadrant --- */
  for ( qptr=quads; *qptr!='\000'; qptr++ ) /* for each character in quads */
   {
   rsign = (-1);  csign = 1;		/* init row,col in user quadrant 1 */
   switch ( *qptr )			/* check for quadrant 1,2,3,4 */
    { default: break;			/* unrecognized, assume quadrant 1 */
      case '4': rsign = 1; break;	/* row,col both pos in quadrant 4 */
      case '3': rsign = 1;		/* row pos, col neg in quadrant 3 */
      case '2': csign = (-1); break; }	/* row,col both neg in quadrant 2 */
   irow = iround(midrow + (double)rsign*(islandscape?yx:xy));
   irow = min2(hirow,max2(lorow,irow));	/* keep irow in bounds */
   icol = iround(midcol + (double)csign*(islandscape?xy:yx));
   icol = min2(hicol,max2(locol,icol));	/* keep icol in bounds */
   if ( msgfp!=NULL && msglevel>=49 )	/* debugging */
     fprintf(msgfp,"\t...imajor=%d; iminor,quad,irow,icol=%d,%c,%d,%d\n",
     imajor,iminor,*qptr,irow,icol);
   if ( irow<0 || irow>=rp->height	/* row outside raster */
   ||   icol<0 || icol>=rp->width )	/* col outside raster */
      {	isokay = 0;			/* signal out-of-bounds pixel */
	continue; }			/* but still try remaining points */
   setpixel(rp,irow,icol,255);		/* set pixel at irow,icol */
   } /* --- end-of-for(qptr) --- */
  } /* --- end-of-for(imajor) --- */
 /* ------------------------------------------------------------------------
 now do it _again_ along minor axis to avoid "gaps"
 ------------------------------------------------------------------------- */
 if ( 1 && iminor>0 )
  for ( iminor=(nminor+1)/2; ; iminor-- )
   {
   /* --- yx is coord along minor axis, xy is "solved" along major axis --- */
   yx  = ((double)iminor);		/* yx = abminor ... 0 */
   if ( yx < 0.0 ) break;		/* negative side symmetrical */
   xy2 = abmajor2*(1.0 - yx*yx/abminor2); /* "solve" ellipse equation */
   xy  = (xy2>0.0? sqrt(xy2) : 0.0);	/* take sqrt if possible */
   imajor = iround(xy);			/* nearest integer */
   /* --- set pixels for each requested quadrant --- */
   for ( qptr=quads; *qptr!='\000'; qptr++ ) /* for each character in quads */
    {
    rsign = (-1);  csign = 1;		/* init row,col in user quadrant 1 */
    switch ( *qptr )			/* check for quadrant 1,2,3,4 */
     { default: break;			/* unrecognized, assume quadrant 1 */
       case '4': rsign = 1; break;	/* row,col both pos in quadrant 4 */
       case '3': rsign = 1;		/* row pos, col neg in quadrant 3 */
       case '2': csign = (-1); break; }	/* row,col both neg in quadrant 2 */
    irow = iround(midrow + (double)rsign*(islandscape?yx:xy));
    irow = min2(hirow,max2(lorow,irow)); /* keep irow in bounds */
    icol = iround(midcol + (double)csign*(islandscape?xy:yx));
    icol = min2(hicol,max2(locol,icol)); /* keep icol in bounds */
    if ( msgfp!=NULL && msglevel>=49 )	/* debugging */
     fprintf(msgfp,"\t...iminor=%d; imajor,quad,irow,icol=%d,%c,%d,%d\n",
     iminor,imajor,*qptr,irow,icol);
    if ( irow<0 || irow>=rp->height	/* row outside raster */
    ||   icol<0 || icol>=rp->width )	/* col outside raster */
      {	isokay = 0;			/* signal out-of-bounds pixel */
	continue; }			/* but still try remaining points */
    setpixel(rp,irow,icol,255);		/* set pixel at irow,icol */
    } /* --- end-of-for(qptr) --- */
   } /* --- end-of-for(iminor) --- */
 } /* --- end-of-if/else(nmajor<1) --- */
return ( isokay );
} /* --- end-of-function circle_raster() --- */


/* ==========================================================================
 * Function:	circle_recurse ( rp,  row0, col0,  row1, col1,
 *		thickness, theta0, theta1 )
 * Purpose:	Recursively draws arc theta0<=theta<=theta1 of the ellipse
 *		in box determined by diagonally opposite corner points
 *		(row0,col0) and (row1,col1), of thickness pixels in raster rp.
 * --------------------------------------------------------------------------
 * Arguments:	rp (I)		raster *  to raster in which an ellipse
 *				will be drawn
 *		row0 (I)	int containing 1st corner row bounding ellipse
 *				(0 is topmost)
 *		col0 (I)	int containing 1st corner col bounding ellipse
 *				(0 is leftmost)
 *		row1 (I)	int containing 2nd corner row bounding ellipse
 *				(rp->height-1 is bottom-most)
 *		col1 (I)	int containing 2nd corner col bounding ellipse
 *				(rp->width-1 is rightmost)
 *		thickness (I)	int containing number of pixels/bits
 *				thick the ellipse arc line will be
 *		theta0 (I)	double containing first angle -360 -> +360
 *		theta1 (I)	double containing second angle -360 -> +360
 *				0=x-axis, positive moving counterclockwise
 * --------------------------------------------------------------------------
 * Returns:	( int )		1 if ellipse drawn okay,
 *				or 0 for any error.
 * --------------------------------------------------------------------------
 * Notes:     o	row0==row1 or col0==col1 are errors
 *	      o	using ellipse equation x^2/a^2 + y^2/b^2 = 1
 *		Then, with x=r*cos(theta), y=r*sin(theta), ellipse
 *		equation is r = ab/sqrt(a^2*sin^2(theta)+b^2*cos^2(theta))
 * ======================================================================= */
/* --- entry point --- */
int	circle_recurse ( raster *rp, int row0, int col0,
	int row1, int col1, int thickness, double theta0, double theta1 )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
/* --- lower-left and upper-right bounding points (in our coords) --- */
int	lorow = min2(row0,row1),	/* lower bounding row (top of box) */
	locol = min2(col0,col1),	/* lower bounding col (left of box)*/
	hirow = max2(row0,row1),	/* upper bounding row (bot of box) */
	hicol = max2(col0,col1);	/* upper bounding col (right of box)*/
/* --- a and b ellipse params --- */
int	width = hicol-locol+1,		/* width of bounding box */
	height= hirow-lorow+1;		/* height of bounding box */
double	a = ((double)width)/2.0,	/* col x=a when row y=0 */
	b = ((double)height)/2.0,	/* row y=b when col x=0 */
	ab=a*b, a2=a*a, b2=b*b;		/* product and squares */
/* --- arc parameters --- */
double	rads = 0.017453292,		/* radians per degree = 1/57.29578 */
	lotheta = rads*dmod(min2(theta0,theta1),360), /* smaller angle */
	hitheta = rads*dmod(max2(theta0,theta1),360), /* larger angle */
	locos=cos(lotheta), losin=sin(lotheta), /* trigs for lotheta */
	hicos=cos(hitheta), hisin=sin(hitheta), /* trigs for hitheta */
	rlo = ab/sqrt(b2*locos*locos+a2*losin*losin), /* r for lotheta */
	rhi = ab/sqrt(b2*hicos*hicos+a2*hisin*hisin), /* r for hitheta */
	xlo=rlo*locos, ylo=rlo*losin,	/*col,row pixel coords for lotheta*/
	xhi=rhi*hicos, yhi=rhi*hisin,	/*col,row pixel coords for hitheta*/
	xdelta=fabs(xhi-xlo), ydelta=fabs(yhi-ylo), /* col,row deltas */
	tolerance = 0.5;		/* convergence tolerance */
/* -------------------------------------------------------------------------
recurse if either delta > tolerance
-------------------------------------------------------------------------- */
if ( ydelta > tolerance			/* row hasn't converged */
||   xdelta > tolerance )		/* col hasn't converged */
  { double midtheta = 0.5*(theta0+theta1); /* mid angle for arc */
    circle_recurse(rp,row0,col0,row1,col1,thickness,theta0,midtheta);  /*lo*/
    circle_recurse(rp,row0,col0,row1,col1,thickness,midtheta,theta1); }/*hi*/
/* -------------------------------------------------------------------------
draw converged point
-------------------------------------------------------------------------- */
else
  { double xcol=0.5*(xlo+xhi), yrow=0.5*(ylo+yhi),    /* relative to center*/
	centerrow = 0.5*((double)(lorow+hirow)),      /* ellipse y-center */
	centercol = 0.5*((double)(locol+hicol)),      /* ellipse x-center */
	midrow=centerrow-yrow, midcol=centercol+xcol; /* pixel coords */
    setpixel(rp,iround(midrow),iround(midcol),255); } /* set midrow,midcol */
return ( 1 );
} /* --- end-of-function circle_recurse() --- */


/* ==========================================================================
 * Function:	bezier_raster ( rp, r0,c0, r1,c1, rt,ct )
 * Purpose:	Recursively draw bezier from r0,c0 to r1,c1
 *		(with tangent point rt,ct) in existing raster rp.
 * --------------------------------------------------------------------------
 * Arguments:	rp (I)		raster *  to raster in which a line
 *				will be drawn
 *		r0 (I)		double containing row at which
 *				bezier will start (0 is topmost)
 *		c0 (I)		double containing col at which
 *				bezier will start (0 is leftmost)
 *		r1 (I)		double containing row at which
 *				bezier will end (rp->height-1 is bottom-most)
 *		c1 (I)		double containing col at which
 *				bezier will end (rp->width-1 is rightmost)
 *		rt (I)		double containing row for tangent point
 *		ct (I)		double containing col for tangent point
 * --------------------------------------------------------------------------
 * Returns:	( int )		1 if line drawn okay,
 *				or 0 for any error.
 * --------------------------------------------------------------------------
 * Notes:     o	Recurses, drawing left- and right-halves of bezier curve
 *		until a point is found
 * ======================================================================= */
/* --- entry point --- */
int	bezier_raster ( raster *rp, double r0, double c0,
	double r1, double c1, double rt, double ct )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
double	delrow = fabs(r1-r0),		/* 0 if same row */
	delcol = fabs(c1-c0),		/* 0 if same col */
	tolerance = 0.5;		/* draw curve when it goes to point*/
double	midrow = 0.5*(r0+r1),		/* midpoint row */
	midcol = 0.5*(c0+c1);		/* midpoint col */
int	irow=0, icol=0;			/* point to be drawn */
int	status = 1;			/* return status */
/* -------------------------------------------------------------------------
recurse if either delta > tolerance
-------------------------------------------------------------------------- */
if ( delrow > tolerance			/* row hasn't converged */
||   delcol > tolerance )		/* col hasn't converged */
  { bezier_raster(rp, r0,c0,		/* left half */
	0.5*(rt+midrow), 0.5*(ct+midcol),
	0.5*(r0+rt), 0.5*(c0+ct) );
    bezier_raster(rp, 0.5*(rt+midrow), 0.5*(ct+midcol), /* right half */
	r1,c1,
	0.5*(r1+rt), 0.5*(c1+ct) );
    return ( 1 ); }
/* -------------------------------------------------------------------------
draw converged point
-------------------------------------------------------------------------- */
/* --- get integer point --- */
irow = iround(midrow);			/* row pixel coord */
icol = iround(midcol);			/* col pixel coord */
/* --- bounds check --- */
if ( irow>=0 && irow<rp->height		/* row in bounds */
&&   icol>=0 && icol<rp->width )	/* col in bounds */
	setpixel(rp,irow,icol,255);	/* so set pixel at irow,icol*/
else	status = 0;			/* bad status if out-of-bounds */
return ( status );
} /* --- end-of-function bezier_raster() --- */


/* ==========================================================================
 * Function:	border_raster ( rp, ntop, nbot, isline, isfree )
 * Purpose:	Allocate a new raster containing a copy of input rp,
 *		along with ntop extra rows at top and nbot at bottom,
 *		and whose width is either adjusted correspondingly,
 *		or is automatically enlarged to a multiple of 8
 *		with original bitmap centered
 * --------------------------------------------------------------------------
 * Arguments:	rp (I)		raster *  to raster on which a border
 *				is to be placed
 *		ntop (I)	int containing number extra rows at top.
 *				if negative, abs(ntop) used, and same
 *				number of extra cols added at left.
 *		nbot (I)	int containing number extra rows at bottom.
 *				if negative, abs(nbot) used, and same
 *				number of extra cols added at right.
 *		isline (I)	int containing 0 to leave border pixels clear
 *				or >0 to draw a line around border of
 *				thickness isline pixels.  See Notes below.
 *		isfree (I)	int containing true to free rp before return
 * --------------------------------------------------------------------------
 * Returns:	( raster * )	ptr to bordered raster,
 *				or NULL for any error.
 * --------------------------------------------------------------------------
 * Notes:     o	The isline arg also controls which sides border lines
 *		are drawn for.  To do this, isline is interpreted as
 *		thickness + 100*sides  so that, e.g., passing isline=601
 *		is interpreted as sides=6 and thickness=1.  And
 *		sides is further interpreted as 1=left side, 2=top,
 *		4=right and 8=bottom.  For example, sides=6 where 6=2+4
 *		draws the top and right borders only.  15 draws all four
 *		sides.  And 0 (no sides value embedded in isline)
 *		draws all four sides, too.
 * ======================================================================= */
/* --- entry point --- */
raster	*border_raster ( raster *rp, int ntop, int nbot,
			int isline, int isfree )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
raster	*new_raster(), *bp=(raster *)NULL;  /*raster back to caller*/
int	rastput();		/* overlay rp in new bordered raster */
int	width  = (rp==NULL?0:rp->width),  /* width of raster */
	height = (rp==NULL?0:rp->height), /* height of raster */
	istopneg=0, isbotneg=0,		/* true if ntop or nbot negative */
	leftmargin = 0;		/* adjust width to whole number of bytes */
int	left=1, top=1, right=1, bot=1;	/* frame sides to draw */
int	delete_raster();		/* free input rp if isfree is true */
/* -------------------------------------------------------------------------
Initialization
-------------------------------------------------------------------------- */
if ( rp == NULL ) goto end_of_job;	/* no input raster provided */
if ( isstring || (1 && rp->height==1) )	/* explicit string signal or infer */
  { bp=rp; goto end_of_job; }		/* return ascii string unchanged */
/* --- check for negative args --- */
if ( ntop < 0 ) { ntop = -ntop; istopneg=1; } /*flip positive and set flag*/
if ( nbot < 0 ) { nbot = -nbot; isbotneg=1; } /*flip positive and set flag*/
/* --- adjust height for ntop and nbot margins --- */
height += (ntop+nbot);			/* adjust height for margins */
/* --- adjust width for left and right margins --- */
if ( istopneg || isbotneg )	/*caller wants nleft=ntop and/or nright=nbot*/
  { /* --- adjust width (and leftmargin) as requested by caller -- */
    if ( istopneg ) { width += ntop; leftmargin = ntop; }
    if ( isbotneg )   width += nbot;  }
else
  { /* --- or adjust width (and leftmargin) to whole number of bytes --- */
    leftmargin = (width%8==0? 0 : 8-(width%8)); /*makes width multiple of 8*/
    width += leftmargin;		/* width now multiple of 8 */
    leftmargin /= 2; }			/* center original raster */
/* --- check which sides to draw --- */
if ( isline > 100 ) {			/* sides arg embedded in isline */
  int iside=0, sides=isline/100;	/* index, sides=1-15 from 101-1599 */
  isline -= 100*sides;			/* and remove sides from isline */
  for ( iside=1; iside<=4; iside++ ) {	/* check left, top, right, bot */
    int shift = sides/2;		/* shift sides left one bit */
    if ( sides == 2*shift )		/* low-order bit is >>not<< set */
      switch ( iside ) {		/* don't draw corresponding side */
        default: break;			/* internal error */
        case 1: left = 0; break;	/* 1 = left side */
        case 2: top  = 0; break;	/* 2 = top side */
        case 3: right= 0; break;	/* 4 = tight side */
        case 4: bot  = 0; break; }	/* 8 = bottom side */
    sides = shift;			/* ready for next side */
    } /* --- end-of-for(iside) --- */
  } /* --- end-of-if(isline>100) --- */
/* -------------------------------------------------------------------------
allocate bordered raster, and embed rp within it
-------------------------------------------------------------------------- */
/* --- allocate bordered raster --- */
if ( (bp=new_raster(width,height,rp->pixsz))  /*allocate bordered raster*/
==   (raster *)NULL ) goto end_of_job;	/* and quit if failed */
/* --- embed rp in it --- */
rastput(bp,rp,ntop,leftmargin,1);	/* rp embedded in bp */
/* -------------------------------------------------------------------------
draw border if requested
-------------------------------------------------------------------------- */
if ( isline )
 { int	irow, icol, nthick=isline;	/*height,width index, line thickness*/
  /* --- draw left- and right-borders --- */
  for ( irow=0; irow<height; irow++ )	/* for each row of bp */
    for ( icol=0; icol<nthick; icol++ )	/* and each pixel of thickness */
      {	if(left){setpixel(bp,irow,icol,255);}		/* left border */
	if(right){setpixel(bp,irow,width-1-icol,255);} } /* right border */
  /* --- draw top- and bottom-borders --- */
  for ( icol=0; icol<width; icol++ )	/* for each col of bp */
    for ( irow=0; irow<nthick; irow++ )	/* and each pixel of thickness */
      {	if(top){setpixel(bp,irow,icol,255);}		/* top border */
	if(bot){setpixel(bp,height-1-irow,icol,255);} }	/* bottom border */
 } /* --- end-of-if(isline) --- */
/* -------------------------------------------------------------------------
free rp if no longer needed
-------------------------------------------------------------------------- */
if ( isfree )					/*caller no longer needs rp*/
  delete_raster(rp);				/* so free it for him */
/* -------------------------------------------------------------------------
Back to caller with bordered raster (or null for any error)
-------------------------------------------------------------------------- */
end_of_job:
  return ( bp );			/* back with bordered or null ptr */
} /* --- end-of-function border_raster() --- */


/* ==========================================================================
 * Function:	backspace_raster ( rp, nback, pback, minspace, isfree )
 * Purpose:	Allocate a new raster containing a copy of input rp,
 *		but with trailing nback columns removed.
 *		If minspace>=0 then (at least) that many columns
 *		of whitespace will be left in place, regardless of nback.
 *		If minspace<0 then existing black pixels will be deleted
 *		as required.
 * --------------------------------------------------------------------------
 * Arguments:	rp (I)		raster *  to raster on which a border
 *				is to be placed
 *		nback (I)	int containing number of columns to
 *				backspace (>=0)
 *		pback (O)	ptr to int returning #pixels actually
 *				backspaced (or NULL to not use)
 *		minspace (I)	int containing number of columns
 *				of whitespace to be left in place
 *		isfree (I)	int containing true to free rp before return
 * --------------------------------------------------------------------------
 * Returns:	( raster * )	ptr to backspaced raster,
 *				or NULL for any error.
 * --------------------------------------------------------------------------
 * Notes:     o	For \! negative space, for \hspace{-10}, etc.
 * ======================================================================= */
/* --- entry point --- */
raster	*backspace_raster ( raster *rp, int nback, int *pback, int minspace,
	int isfree )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
raster	*new_raster(), *bp=(raster *)NULL;  /* raster returned to caller */
int	delete_raster();		/* free input rp if isfree is true */
int	width  = (rp==NULL?0:rp->width),  /* original width of raster */
	height = (rp==NULL?0:rp->height), /* height of raster */
	mback = nback,			/* nback adjusted for minspace */
	newwidth = width,		/* adjusted width after backspace */
	icol=0, irow=0;			/* col,row index */
if ( rp == NULL ) goto end_of_job;	/* no input given */
/* -------------------------------------------------------------------------
locate rightmost column of rp containing ink, and determine backspaced width
-------------------------------------------------------------------------- */
/* --- locate rightmost column of rp containing ink --- */
if ( minspace >= 0 )			/* only needed if given minspace */
 for ( icol=width-1; icol>=0; icol-- )	/* find first non-empty col in row */
  for ( irow=0; irow<height; irow++ )	/* for each row inside rp */
   if ( getpixel(rp,irow,icol) != 0 ) {	/* found a set pixel */
     int whitecols = (width-1) - icol;	/* #cols containing white space */
     mback = min2(nback,max2(0,whitecols-minspace)); /*leave minspace cols*/
     goto gotright; }			/* no need to look further */
/* --- determine width of new backspaced raster --- */
gotright:				/* found col with ink (or rp empty)*/
  if ( mback > width ) mback = width;	/* can't backspace before beginning*/
  newwidth = max2(1,width-mback);	/* #cols in backspaced raster */
  if ( pback != NULL ) *pback = width-newwidth; /* caller wants #pixels */
/* -------------------------------------------------------------------------
allocate new raster and fill it with leftmost cols of rp
-------------------------------------------------------------------------- */
/* --- allocate backspaced raster --- */
if ( (bp=new_raster(newwidth,height,rp->pixsz)) /*allocate backspaced raster*/
==   (raster *)NULL ) goto end_of_job;	/* and quit if failed */
/* --- fill new raster --- */
if ( 1 || width-nback > 0 )		/* don't fill 1-pixel wide empty bp*/
 for ( icol=0; icol<newwidth; icol++ )	/* find first non-empty col in row */
  for ( irow=0; irow<height; irow++ )	/* for each row inside rp */
    { int value = getpixel(rp,irow,icol); /* original pixel at irow,icol */
      setpixel(bp,irow,icol,value); }	/* saved in backspaced raster */
/* -------------------------------------------------------------------------
Back to caller with backspaced raster (or null for any error)
-------------------------------------------------------------------------- */
end_of_job:
  if ( msgfp!=NULL && msglevel>=999 ) { fprintf(msgfp, /* diagnostics */
   "backspace_raster> nback=%d,minspace=%d,mback=%d, width:old=%d,new=%d\n",
   nback,minspace,mback,width,newwidth); fflush(msgfp); }
  if ( isfree && bp!=NULL ) delete_raster(rp); /* free original raster */
  return ( bp );			/* back with backspaced or null ptr*/
} /* --- end-of-function backspace_raster() --- */


/* ==========================================================================
 * Function:	type_raster ( rp, fp )
 * Purpose:	Emit an ascii dump representing rp, on fp.
 * --------------------------------------------------------------------------
 * Arguments:	rp (I)		ptr to raster struct for which an
 *				ascii dump is to be constructed.
 *		fp (I)		File ptr to output device (defaults to
 *				stdout if passed as NULL).
 * --------------------------------------------------------------------------
 * Returns:	( int )		1 if completed successfully,
 *				or 0 otherwise (for any error).
 * --------------------------------------------------------------------------
 * Notes:
 * ======================================================================= */
/* --- entry point --- */
int	type_raster ( raster *rp, FILE *fp )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
static	int display_width = 72;		/* max columns for display */
static	char display_chars[16] =	/* display chars for bytemap */
	{ ' ','1','2','3','4','5','6','7','8','9','A','B','C','D','E','*' };
char	scanline[133];			/* ascii image for one scan line */
int	scan_width;			/* #chars in scan (<=display_width)*/
int	irow, locol,hicol=(-1);		/* height index, width indexes */
raster	*gftobitmap(), *bitmaprp=rp;	/* convert .gf to bitmap if needed */
int	delete_raster();		/*free bitmap converted for display*/
/* --------------------------------------------------------------------------
initialization
-------------------------------------------------------------------------- */
/* --- redirect null fp --- */
if ( fp == (FILE *)NULL ) fp = stdout;	/* default fp to stdout if null */
if ( msglevel >= 999 ) { fprintf(fp,	/* debugging diagnostics */
  "type_raster> width=%d height=%d ...\n",
  rp->width,rp->height); fflush(fp); }
/* --- check for ascii string --- */
if ( isstring				/* pixmap has string, not raster */
||   (0 && rp->height==1) )		/* infer input rp is a string */
 {
 char *string = (char *)(rp->pixmap);	/*interpret pixmap as ascii string*/
 int width = strlen(string);		/* #chars in ascii string */
 while ( width > display_width-2 )	/* too big for one line */
  { fprintf(fp,"\"%.*s\"\n",display_width-2,string); /*display leading chars*/
    string += (display_width-2);	/* bump string past displayed chars*/
    width -= (display_width-2); }	/* decrement remaining width */
 fprintf(fp,"\"%.*s\"\n",width,string);	/* display trailing chars */
 return ( 1 );
 } /* --- end-of-if(isstring) --- */
/* --------------------------------------------------------------------------
display ascii dump of bitmap image (in segments if display_width < rp->width)
-------------------------------------------------------------------------- */
if ( rp->format == 2			/* input is .gf-formatted */
||   rp->format == 3 )
  bitmaprp = gftobitmap(rp);		/* so convert it for display */
if ( bitmaprp != NULL )			/* if we have image for display */
 while ( (locol=hicol+1) < rp->width )	/*start where prev segment left off*/
  {
  /* --- set hicol for this pass (locol set above) --- */
  hicol += display_width;		/* show as much as display allows */
  if (hicol >= rp->width) hicol = rp->width - 1; /*but not more than raster*/
  scan_width = hicol-locol+1;		/* #chars in this scan */
  if ( locol > 0 ) fprintf(fp,"----------\n"); /*separator between segments*/
  /* ------------------------------------------------------------------------
  display all scan lines for this local...hicol segment range
  ------------------------------------------------------------------------ */
  for ( irow=0; irow<rp->height; irow++ )  /* all scan lines for col range */
    {
    /* --- allocations and declarations --- */
    int	ipix,				/* pixmap[] index for this scan */
	lopix = irow*rp->width + locol;	/*first pixmap[] pixel in this scan*/
    /* --- set chars in scanline[] based on pixels in rp->pixmap[] --- */
    for ( ipix=0; ipix<scan_width; ipix++ ) /* set each char */
      if ( bitmaprp->pixsz == 1 )	/*' '=0 or '*'=1 to display bitmap*/
	scanline[ipix]=(getlongbit(bitmaprp->pixmap,lopix+ipix)==1? '*':'.');
      else				/* should have a bytemap */
       if ( bitmaprp->pixsz == 8 )	/* double-check pixsz for bytemap */
	{ int pixval = (int)((bitmaprp->pixmap)[lopix+ipix]), /*byte value*/
	  ichar = min2(15,pixval/16);	/* index for ' ', '1'...'e', '*' */
	  scanline[ipix] = display_chars[ichar]; } /*set ' ' for 0-15, etc*/
    /* --- display completed scan line --- */
    fprintf(fp,"%.*s\n",scan_width,scanline);	
    } /* --- end-of-for(irow) --- */
  } /* --- end-of-while(hicol<rp->width) --- */
/* -------------------------------------------------------------------------
Back to caller with 1=okay, 0=failed.
-------------------------------------------------------------------------- */
if ( rp->format == 2			/* input was .gf-format */
||   rp->format == 3 )
  if ( bitmaprp != NULL )		/* and we converted it for display */
    delete_raster(bitmaprp);		/* no longer needed, so free it */
return ( 1 );
} /* --- end-of-function type_raster() --- */


/* ==========================================================================
 * Function:	type_bytemap ( bp, grayscale, width, height, fp )
 * Purpose:	Emit an ascii dump representing bp, on fp.
 * --------------------------------------------------------------------------
 * Arguments:	bp (I)		intbyte * to bytemap for which an
 *				ascii dump is to be constructed.
 *		grayscale (I)	int containing #gray shades, 256 for 8-bit
 *		width (I)	int containing #cols in bytemap
 *		height (I)	int containing #rows in bytemap
 *		fp (I)		File ptr to output device (defaults to
 *				stdout if passed as NULL).
 * --------------------------------------------------------------------------
 * Returns:	( int )		1 if completed successfully,
 *				or 0 otherwise (for any error).
 * --------------------------------------------------------------------------
 * Notes:
 * ======================================================================= */
/* --- entry point --- */
int	type_bytemap ( intbyte *bp, int grayscale,
			int width, int height, FILE *fp )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
static	int display_width = 72;		/* max columns for display */
int	byte_width = 3,			/* cols to display byte (ff+space) */
	maxbyte = 0;			/* if maxbyte<16, set byte_width=2 */
int	white_byte = 0,			/* show dots for white_byte's */
	black_byte = grayscale-1;	/* show stars for black_byte's */
char	scanline[133];			/* ascii image for one scan line */
int	scan_width,			/* #chars in scan (<=display_width)*/
	scan_cols;			/* #cols in scan (hicol-locol+1) */
int	ibyte,				/* bp[] index */
	irow, locol,hicol=(-1);		/* height index, width indexes */
/* --------------------------------------------------------------------------
initialization
-------------------------------------------------------------------------- */
/* --- redirect null fp --- */
if ( fp == (FILE *)NULL ) fp = stdout;	/* default fp to stdout if null */
/* --- check for ascii string --- */
if ( isstring )				/* bp has ascii string, not raster */
 { width = strlen((char *)bp);		/* #chars in ascii string */
   height = 1; }			/* default */
/* --- see if we can get away with byte_width=1 --- */
for ( ibyte=0; ibyte<width*height; ibyte++ )  /* check all bytes */
  { int	byteval = (int)bp[ibyte];	/* current byte value */
    if ( byteval < black_byte )		/* if it's less than black_byte */
      maxbyte = max2(maxbyte,byteval); } /* then find max non-black value */
if ( maxbyte < 16 )			/* bytevals will fit in one column */
  byte_width = 1;			/* so reset display byte_width */
/* --------------------------------------------------------------------------
display ascii dump of bitmap image (in segments if display_width < rp->width)
-------------------------------------------------------------------------- */
while ( (locol=hicol+1) < width )	/*start where prev segment left off*/
  {
  /* --- set hicol for this pass (locol set above) --- */
  hicol += display_width/byte_width;	/* show as much as display allows */
  if (hicol >= width) hicol = width - 1; /* but not more than bytemap */
  scan_cols = hicol-locol+1;		/* #cols in this scan */
  scan_width = byte_width*scan_cols;	/* #chars in this scan */
  if ( locol>0 && !isstring ) fprintf(fp,"----------\n"); /* separator */
  /* ------------------------------------------------------------------------
  display all scan lines for this local...hicol segment range
  ------------------------------------------------------------------------ */
  for ( irow=0; irow<height; irow++ )	/* all scan lines for col range */
    {
    /* --- allocations and declarations --- */
    int  lobyte = irow*width + locol;	/* first bp[] byte in this scan */
    char scanbyte[32];			/* sprintf() buffer for byte */
    /* --- set chars in scanline[] based on bytes in bytemap bp[] --- */
    memset(scanline,' ',scan_width);	/* blank out scanline */
    for ( ibyte=0; ibyte<scan_cols; ibyte++ ) /* set chars for each col */
      {	int byteval = (int)bp[lobyte+ibyte];  /* value of current byte */
	memset(scanbyte,'.',byte_width); /* dot-fill scanbyte */
	if ( byteval == black_byte )	/* but if we have a black byte */
	  memset(scanbyte,'*',byte_width); /* star-fill scanbyte instead */
	if ( byte_width > 1 )		/* don't blank out single char */
	  scanbyte[byte_width-1] = ' ';	/* blank-fill rightmost character */
	if ( byteval != white_byte	/* format bytes that are non-white */
	&&   byteval != black_byte )	/* and that are non-black */
	  sprintf(scanbyte,"%*x ",max2(1,byte_width-1),byteval); /*hex-format*/
	memcpy(scanline+ibyte*byte_width,scanbyte,byte_width); } /*in line*/
    /* --- display completed scan line --- */
    fprintf(fp,"%.*s\n",scan_width,scanline);	
    } /* --- end-of-for(irow) --- */
  } /* --- end-of-while(hicol<width) --- */
/* -------------------------------------------------------------------------
Back to caller with 1=okay, 0=failed.
-------------------------------------------------------------------------- */
return ( 1 );
} /* --- end-of-function type_bytemap() --- */


/* ==========================================================================
 * Function:	xbitmap_raster ( rp, fp )
 * Purpose:	Emit a mime xbitmap representing rp, on fp.
 * --------------------------------------------------------------------------
 * Arguments:	rp (I)		ptr to raster struct for which a mime
 *				xbitmap is to be constructed.
 *		fp (I)		File ptr to output device (defaults to
 *				stdout if passed as NULL).
 * --------------------------------------------------------------------------
 * Returns:	( int )		1 if completed successfully,
 *				or 0 otherwise (for any error).
 * --------------------------------------------------------------------------
 * Notes:
 * ======================================================================= */
/* --- entry point --- */
int	xbitmap_raster ( raster *rp, FILE *fp )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
char	*title = "image";		/* dummy title */
int	hex_bitmap();			/* dump bitmap as hex bytes */
/* --------------------------------------------------------------------------
emit text to display mime xbitmap representation of rp->bitmap image
-------------------------------------------------------------------------- */
/* --- first redirect null fp --- */
if ( fp == (FILE *)NULL ) fp = stdout;	/* default fp to stdout if null */
/* --- check for ascii string --- */
if ( isstring )				/* pixmap has string, not raster */
 return ( 0 );				/* can't handle ascii string */
/* --- emit prologue strings and hex dump of bitmap for mime xbitmap --- */
fprintf( fp, "Content-type: image/x-xbitmap\n\n" );
fprintf( fp, "#define %s_width %d\n#define %s_height %d\n",
	title,rp->width, title,rp->height );
fprintf( fp, "static char %s_bits[] = {\n", title );
hex_bitmap(rp,fp,0,0);			/* emit hex dump of bitmap bytes */
fprintf (fp,"};\n");			/* ending with "};" for C array */
/* -------------------------------------------------------------------------
Back to caller with 1=okay, 0=failed.
-------------------------------------------------------------------------- */
return ( 1 );
} /* --- end-of-function xbitmap_raster() --- */


/* ==========================================================================
 * Function:	type_pbmpgm ( rp, ptype, file )
 * Purpose:	Write pbm or pgm image of rp to file
 * --------------------------------------------------------------------------
 * Arguments:	rp (I)		ptr to raster struct for which
 *				a pbm/pgm file is to be written.
 *		ptype (I)	int containing 1 for pbm, 2 for pgm, or
 *				0 to determine ptype from values in rp
 *		file (I)	ptr to null-terminated char string
 *				containing name of fuke to be written
 *				(see notes below).
 * --------------------------------------------------------------------------
 * Returns:	( int )		total #bytes written,
 *				or 0 for any error.
 * --------------------------------------------------------------------------
 * Notes:     o	(a) If file==NULL, output is written to stdout;
 *		(b) if *file=='\000' then file is taken as the
 *		    address of an output buffer to which output
 *		    is written (and is followed by a terminating
 *		    '\0' which is not counted in #bytes returned);
 *		(c) otherwise file is the filename (opened and
 *		    closed internally) to which output is written,
 *		    except that any final .ext extension is replaced
 *		    by .pbm or .pgm depending on ptype.
 * ======================================================================= */
/* --- entry point --- */
int	type_pbmpgm ( raster *rp, int ptype, char *file )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
int	isokay=0, nbytes=0;	/* completion flag, total #bytes written */
int	irow=0, jcol=0;		/*height(row), width(col) indexes in raster*/
int	pixmin=9999, pixmax=(-9999), /* min, max pixel value in raster */
	ngray = 0;		/* #gray scale values */
FILE	/* *fopen(), */ *fp=NULL; /* pointer to output file (or NULL) */
char	outline[1024], outfield[256], /* output line, field */
	cr[16] = "\n\000";	/* cr at end-of-line */
int	maxlinelen = 70;	/* maximum allowed line length */
int	pixfrac=6;		/* use (pixmax-pixmin)/pixfrac as step */
static	char
	*suffix[] = { NULL, ".pbm", ".pgm" },	/* file.suffix[ptype] */
	*magic[] = { NULL, "P1", "P2" },	/*identifying "magic number"*/
	*mode[] = { NULL, "w", "w" };		/* fopen() mode[ptype] */
/* -------------------------------------------------------------------------
check input, determine grayscale,  and set up output file if necessary
-------------------------------------------------------------------------- */
/* --- check input args --- */
if ( rp == NULL ) goto end_of_job;	/* no input raster provided */
if ( ptype != 0 )			/* we'll determine ptype below */
 if ( ptype<1 || ptype>2 ) goto end_of_job; /*invalid output graphic format*/
/* --- determine largest (and smallest) value in pixmap --- */
for ( irow=0; irow<rp->height; irow++ )	/* for each row, top-to-bottom */
 for ( jcol=0; jcol<rp->width; jcol++ )	/* for each col, left-to-right */
  { int	pixval = getpixel(rp,irow,jcol);  /* value of pixel at irow,jcol */
    pixmin = min2(pixmin,pixval);	/* new minimum */
    pixmax = max2(pixmax,pixval); }	/* new maximum */
ngray = 1 + (pixmax-pixmin);		/* should be 2 for b/w bitmap */
if ( ptype == 0 )			/* caller wants us to set ptype */
  ptype = (ngray>=3?2:1);		/* use grayscale if >2 shades */
/* --- open output file if necessary --- */
if ( file == NULL ) fp = stdout;	/*null ptr signals output to stdout*/
else if ( *file != '\000' ) {		/* explicit filename provided, so...*/
  char	fname[512], *pdot=NULL;		/* file.ext, ptr to last . in fname*/
  strncpy(fname,file,255);		/* local copy of file name */
  fname[255] = '\000';			/* make sure it's null terminated */
  if ( (pdot=strrchr(fname,'.')) == NULL ) /*no extension on original name*/
    strcat(fname,suffix[ptype]);	/* so add extension */
  else					/* we already have an extension */
    strcpy(pdot,suffix[ptype]);		/* so replace original extension */
  if ( (fp = fopen(fname,mode[ptype]))	/* open output file */
  ==   (FILE *)NULL ) goto end_of_job;	/* quit if failed to open */
  } /* --- ens-of-if(*file!='\0') --- */
/* -------------------------------------------------------------------------
emit http headers if running as cgi
-------------------------------------------------------------------------- */
/* --- emit mime content-type line --- */
if ( isquery && fp==stdout )		/* writing to http server */
 { fprintf( stdout, "Cache-Control: max-age=9999\n" );
   fprintf( stdout, "Content-type: text/plain\n\n" ); }
/* -------------------------------------------------------------------------
format and write header
-------------------------------------------------------------------------- */
/* --- format header info --- */
*outline = '\000';			/* initialize line buffer */
strcat(outline,magic[ptype]);		/* begin file with "magic number" */
strcat(outline,cr);			/* followed by cr to end line */
sprintf(outfield,"%d %d",rp->width,rp->height); /* format width and height */
strcat(outline,outfield);		/* add width and height to header */
strcat(outline,cr);			/* followed by cr to end line */
if ( ptype == 2 )			/* need max grayscale value */
  { sprintf(outfield,"%d",pixmax);	/* format maximum pixel value */
    strcat(outline,outfield);		/* add max value to header */
    strcat(outline,cr); }		/* followed by cr to end line */
/* --- write header to file or memory buffer --- */
if ( fp == NULL )			/* if we have no open file... */
  strcat(file,outline);			/* add header to caller's buffer */
else					/* or if we have an open file... */
  if ( fputs(outline,fp)		/* try writing header to open file */
  ==   EOF ) goto end_of_job;		/* return with error if failed */
nbytes += strlen(outline);		/* bump output byte count */
/* -------------------------------------------------------------------------
format and write pixels
-------------------------------------------------------------------------- */
*outline = '\000';			/* initialize line buffer */
for ( irow=0; irow<=rp->height; irow++ ) /* for each row, top-to-bottom */
 for ( jcol=0; jcol<rp->width; jcol++ )	{ /* for each col, left-to-right */
  /* --- format value at irow,jcol--- */
  *outfield = '\000';			/* init empty field */
  if ( irow < rp->height ) {		/* check row index */
    int	pixval = getpixel(rp,irow,jcol);  /* value of pixel at irow,jcol */
    if ( ptype == 1 )			/* pixval must be 1 or 0 */
      pixval = (pixval>pixmin+((pixmax-pixmin)/pixfrac)?1:0);
    sprintf(outfield,"%d ",pixval); }	/* format pixel value */
  /* --- write line if this value won't fit on it (or last line) --- */
  if ( strlen(outline)+strlen(outfield)+strlen(cr) >= maxlinelen /*won't fit*/
  ||   irow >= rp->height ) {		/* force writing last line */
    strcat(outline,cr);			/* add cr to end current line */
    if ( fp == NULL )			/* if we have no open file... */
      strcat(file,outline);		/* add header to caller's buffer */
    else				/* or if we have an open file... */
      if ( fputs(outline,fp)		/* try writing header to open file */
      ==   EOF ) goto end_of_job;	/* return with error if failed */
    nbytes += strlen(outline);		/* bump output byte count */
    *outline = '\000';			/* re-initialize line buffer */
    } /* --- end-of-if(strlen>=maxlinelen) --- */
  if ( irow >= rp->height ) break;	/* done after writing last line */
  /* --- concatanate value to line -- */
  strcat(outline,outfield);		/* concatanate value to line */
  } /* --- end-of-for(jcol,irow) --- */
isokay = 1;				/* signal successful completion */
/* -------------------------------------------------------------------------
Back to caller with total #bytes written, or 0=failed.
-------------------------------------------------------------------------- */
end_of_job:
  if ( fp != NULL			/* output written to an open file */
  &&   fp != stdout )			/* and it's not just stdout */
    fclose(fp);				/* so close file before returning */
  return ( (isokay?nbytes:0) );		/*back to caller with #bytes written*/
} /* --- end-of-function type_pbmpgm() --- */


/* ==========================================================================
 * Function:	read_pbm ( fp, sf )
 * Purpose:	read already-opened file (or pipe) containing pbm,
 *		and return subraster corresponding to its bitmap image
 * --------------------------------------------------------------------------
 * Arguments:	fp (I)		FILE *ptr to already-opened file or pipe
 *				(the latter for \mathtex{} and maybe others)
 *		sf (I)		double containing "shrink factor" .01<sf<.99
 *				to be applied to image
 * --------------------------------------------------------------------------
 * Returns:	( subraster * )	ptr to subraster representing image of pbm
 *				or NULL for any error.
 * --------------------------------------------------------------------------
 * Notes:     o
 * ======================================================================= */
/* --- entry point --- */
subraster *read_pbm ( FILE *fp, double sf )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
subraster *new_subraster(), *sp = NULL;	/* subraster corresponding to pbm */
raster	*rp = NULL;			/* sp->image raster in subraster */
raster	*ssrp=NULL, *imgsupsamp();	/* supersampled/shrunk rp */
int	delete_subraster(),		/* in case of problem/error */
	delete_raster();		/* in case we shrink/supersample rp */
unsigned char *pixels = NULL;		/* pixels returned in pbm image */
char	fline[8192],			/* fgets(fline,8190,fp) buffer */
	*lineptr=NULL, *endptr=NULL;	/* strtol ptr to 1st char after num*/
char	*magicnumber = "P1";		/* filetype identification */
int	gotmagic = 0;			/* set true when magicnumber found */
int	ncols=0, nrows=0,		/* image width,height from wget */
	jcol=0, irow=0,			/* col~width, row~height indexes */
	ipixel=0, npixels=0,		/* pixel index, npixels=ncols*nrows */
	pixval=0;			/* pixval=strtol() from pipeline */
int	pixsz = 1,			/* bitmap in returned raster's pixmap*/
	ssgrayscale = 2; /*256;*/	/* grayscale for imgsupsamp() */
/* ---
 * check input
 * -------------- */
if ( fp == NULL ) goto end_of_job;
/* ---
 * read the first few lines to get width, height
 * ------------------------------------------------ */
while ( 1 ) {				/* read lines (shouldn't find eof) */
  if ( fgets(fline,8190,fp) == NULL ) goto end_of_job; /* premature eof */
  if ( gotmagic ) {			/* next line has "width height" */
    char *p = strchr(fline,(int)('#')); /* first look for comment char */
    if ( p != (char *)NULL ) *p = '\000'; /* and terminate line at comment */
    p = fline;  skipwhite(p);		/* now find first non-whitespace char*/
    if ( *p == '\000' ) continue;	/* and skip empty lines */
    ncols = (int)strtol(fline,&endptr,10); /* convert width */
    nrows = (int)strtol(endptr,NULL,10); /* and convert height */
    break; }				/* and we're ready to pixelize... */
  if ( strstr(fline,magicnumber) != NULL ) /* found magicnumber */
    gotmagic = 1;			/* set flag */
  } /* --- end-of-while(1) --- */
/* ---
 * allocate pixels and xlate remainder of pipe
 * ---------------------------------------------- */
/* --- allocate pixels --- */
npixels = nrows*ncols;			/* that's what we need */
if ( npixels<1 || npixels>9999999 ) goto end_of_job; /* sanity check */
if ( (pixels = (unsigned char *)malloc(npixels)) /*ask for it if not insane*/
==   NULL ) goto end_of_job;		/* and quit if request denied */
/* --- xlate remainder of pipe --- */
ipixel = 0;				/* start with first pixel */
while ( 1 ) {				/* read lines (shouldn't find eof) */
  /* --- read next line --- */
  if ( fgets(fline,511,fp) == NULL ) {	/* premature eof */
    free(pixels);  pixels=NULL;		/* failed to xlate completely */
    goto end_of_job; }			/* free, reset return ptr, quit */
  /* --- xlate 0's and 1's on the line --- */
  lineptr = fline;			/* start at beginning of line */
  while ( ipixel < npixels ) {
    while ( *lineptr != '\000' )	/* skip leading non-digits */
      if ( isdigit(*lineptr) ) break;	/* done at first digit */
      else lineptr++;			/* or skip leading non-digit */
    if ( *lineptr == '\000' ) break;	/* no more numbers on this line */
    pixval = (int)strtol(lineptr,&endptr,10); /*convert next num to 0 or 1*/
    pixels[ipixel++] = (unsigned char)pixval; /*i.e., let's hope it's 0 or 1*/
    lineptr = endptr;			/* move on to next number */
    } /* --- end-of-while(ipixels<npixels) --- */
  if ( ipixel >= npixels ) break;	/* pixelized image complete */
  } /* --- end-of-while(1) --- */
/* ---
 * create subraster for pixelized image
 * --------------------------------------- */
if ( (sp = new_subraster(ncols,nrows,pixsz)) /* allocate/init subraster */
==   NULL ) goto end_of_job;		/* or quit if failed */
if ( (rp = sp->image)			/* image raster in new subraster */
==   NULL ) {				/* apparently some internal problem */
  delete_subraster(sp);			/* free "buggy" subraster */
  sp=NULL; goto end_of_job; }		/* return error to caller */
sp->type = IMAGERASTER;			/* set raster type as image */
sp->baseline = (nrows+1)/2;		/* default baseline at image center */
sp->toprow = sp->leftcol = 0;		/* init/unused */
ipixel = 0;				/* start with first pixel */
/* --- copy 0/1 pixels from pbm to sp->image raster --- */
for ( irow=0; irow<nrows; irow++ )	/* for each row, top-to-bottom */
  for ( jcol=0; jcol<ncols; jcol++ ) {	/* for each col, left-to-right */
    pixval = (int)pixels[ipixel++];	/* pbm image stored row-wise */
    setpixel(rp,irow,jcol,pixval);	/* set corresponding value in image */
    } /* --- end-of-for(irow,jcol) --- */
/* --- apply sf (shrink factor) if requested --- */
if ( sf>0.01 && sf<0.99 ) {		/* valid sf means shrink requested */
  if ( (ssrp = imgsupsamp(rp,sf,ssgrayscale)) /* supersample rp to shrink it */
  != NULL ) {				/* succeeded, so replace rp with ssrp*/
    /* printf("read_pbm> imgsupsamp() succeeded\n"); */
    sp->image = ssrp;			/* replaced rp with ssrp */
    sp->baseline = (int)( (sf*((double)nrows)+1.0)/2.0 + 0.5 );
    delete_raster(rp); }		/* free no-longer-needed rp */
  /* else printf("read_pbm> imgsupsamp() failed\n"); */
  } /* --- end-of-if(sf>.01&&sf<.99) --- */
end_of_job:
  if ( pixels != NULL ) free((void *)pixels); /* raster's pixmap has image */
  return ( sp );			/*back to caller with pbm subraster*/
} /* --- end-of-function read_pbm() --- */


/* ==========================================================================
 * Function:	cstruct_chardef ( cp, fp, col1 )
 * Purpose:	Emit a C struct of cp on fp, starting in col1.
 * --------------------------------------------------------------------------
 * Arguments:	cp (I)		ptr to chardef struct for which
 *				a C struct is to be generated.
 *		fp (I)		File ptr to output device (defaults to
 *				stdout if passed as NULL).
 *		col1 (I)	int containing 0...65; output lines
 *				are preceded by col1 blanks.
 * --------------------------------------------------------------------------
 * Returns:	( int )		1 if completed successfully,
 *				or 0 otherwise (for any error).
 * --------------------------------------------------------------------------
 * Notes:
 * ======================================================================= */
/* --- entry point --- */
int	cstruct_chardef ( chardef *cp, FILE *fp, int col1 )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
char	field[64];		/* field within output line */
int	cstruct_raster(),	/* emit a raster */
	emit_string();		/* emit a string and comment */
/* -------------------------------------------------------------------------
emit   charnum, location, name  /  hirow, hicol,  lorow, locol
-------------------------------------------------------------------------- */
/* --- charnum, location, name --- */
sprintf(field,"{ %3d,%5d,\n", cp->charnum,cp->location);  /*char#,location*/
emit_string ( fp, col1, field, "character number, location");
/* --- toprow, topleftcol,   botrow, botleftcol  --- */
sprintf(field,"  %3d,%2d,  %3d,%2d,\n",		/* format... */
  cp->toprow,cp->topleftcol,			/* toprow, topleftcol, */
  cp->botrow,cp->botleftcol);			/* and botrow, botleftcol */
emit_string ( fp, col1, field, "topleft row,col, and botleft row,col");
/* -------------------------------------------------------------------------
emit raster and chardef's closing brace, and then return to caller
-------------------------------------------------------------------------- */
cstruct_raster(&cp->image,fp,col1+4);		/* emit raster */
emit_string ( fp, 0, "  }", NULL);		/* emit closing brace */
return ( 1 );			/* back to caller with 1=okay, 0=failed */
} /* --- end-of-function cstruct_chardef() --- */


/* ==========================================================================
 * Function:	cstruct_raster ( rp, fp, col1 )
 * Purpose:	Emit a C struct of rp on fp, starting in col1.
 * --------------------------------------------------------------------------
 * Arguments:	rp (I)		ptr to raster struct for which
 *				a C struct is to be generated.
 *		fp (I)		File ptr to output device (defaults to
 *				stdout if passed as NULL).
 *		col1 (I)	int containing 0...65; output lines
 *				are preceded by col1 blanks.
 * --------------------------------------------------------------------------
 * Returns:	( int )		1 if completed successfully,
 *				or 0 otherwise (for any error).
 * --------------------------------------------------------------------------
 * Notes:
 * ======================================================================= */
/* --- entry point --- */
int	cstruct_raster ( raster *rp, FILE *fp, int col1 )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
char	field[64];		/* field within output line */
char	typecast[64] = "(pixbyte *)"; /* type cast for pixmap string */
int	hex_bitmap();		/* to emit raster bitmap */
int	emit_string();		/* emit a string and comment */
/* -------------------------------------------------------------------------
emit width and height
-------------------------------------------------------------------------- */
sprintf(field,"{ %2d,  %3d,%2d,%2d, %s\n", /* format width,height,pixsz */
	rp->width,rp->height,rp->format,rp->pixsz,typecast);
emit_string ( fp, col1, field, "width,ht, fmt,pixsz,map...");
/* -------------------------------------------------------------------------
emit bitmap and closing brace, and return to caller
-------------------------------------------------------------------------- */
hex_bitmap(rp,fp,col1+2,1);	/* emit bitmap */
emit_string ( fp, 0, " }", NULL); /* emit closing brace */
return ( 1 );			/* back to caller with 1=okay, 0=failed */
} /* --- end-of-function cstruct_raster() --- */


/* ==========================================================================
 * Function:	hex_bitmap ( rp, fp, col1, isstr )
 * Purpose:	Emit a hex dump of the bitmap of rp on fp, starting in col1.
 *		If isstr (is string) is true, the dump is of the form
 *			"\x01\x02\x03\x04\x05..."
 *		Otherwise, if isstr is false, the dump is of the form
 *			0x01,0x02,0x03,0x04,0x05...
 * --------------------------------------------------------------------------
 * Arguments:	rp (I)		ptr to raster struct for which
 *				a hex dump is to be constructed.
 *		fp (I)		File ptr to output device (defaults to
 *				stdout if passed as NULL).
 *		col1 (I)	int containing 0...65; output lines
 *				are preceded by col1 blanks.
 *		isstr (I)	int specifying dump format as described above
 * --------------------------------------------------------------------------
 * Returns:	( int )		1 if completed successfully,
 *				or 0 otherwise (for any error).
 * --------------------------------------------------------------------------
 * Notes:
 * ======================================================================= */
/* --- entry point --- */
int	hex_bitmap ( raster *rp, FILE *fp, int col1, int isstr )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
int	ibyte,				/* pixmap[ibyte] index */
	nbytes = pixbytes(rp);		/*#bytes in bitmap or .gf-formatted*/
char	stub[64]="                                ";/* col1 leading blanks */
int	linewidth = 64,			/* (roughly) rightmost column */
	colwidth = (isstr? 4:5);	/* #cols required for each byte */
int	ncols = (linewidth-col1)/colwidth; /* new line after ncols bytes */
/* --------------------------------------------------------------------------
initialization
-------------------------------------------------------------------------- */
/* --- redirect null fp --- */
if ( fp == (FILE *)NULL ) fp = stdout;	/* default fp to stdout if null */
/* --- emit initial stub if wanted --- */
if ( col1 > 0 ) fprintf(fp,"%.*s",col1,stub); /* stub preceding 1st line */
/* --------------------------------------------------------------------------
emit hex dump of rp->bitmap image
-------------------------------------------------------------------------- */
if ( isstr ) fprintf(fp,"\"");		/* opening " before first line */
for ( ibyte=0; ibyte<nbytes; ibyte++ )	/* one byte at a time */
  {
  /* --- display a byte as hex char or number, depending on isstr --- */
  if ( isstr )				/* string format wanted */
    fprintf(fp,"\\x%02x",(rp->pixmap)[ibyte]);	/*print byte as hex char*/
  else					/* comma-separated format wanted */
    fprintf(fp,"0x%02x",(rp->pixmap)[ibyte]);	/*print byte as hex number*/
  /* --- add a separator and newline, etc, as necessary --- */
  if ( ibyte < nbytes-1)		/* not the last byte yet */
    {
    if ( !isstr ) fprintf(fp,",");	/* follow hex number with comma */
    if ( (ibyte+1)%ncols==0 ) {		/* need new line after every ncols */
      if ( !isstr )			/* for hex numbers format ... */
	fprintf(fp,"\n%.*s",col1,stub);	/* ...just need newline and stub */
      else				/* for string format... */
	fprintf(fp,"\"\n%.*s\"",col1,stub); } /*...need closing, opening "s*/
    } /* --- end-of-if(ibyte<nbytes-1) --- */
  } /* --- end-of-for(ibyte) --- */
if ( isstr ) fprintf(fp,"\"");		/* closing " after last line */
return ( 1 );				/* back with 1=okay, 0=failed */
} /* --- end-of-function hex_bitmap() --- */


/* ==========================================================================
 * Function:	emit_string ( fp, col1, string, comment )
 * Purpose:	Emit string on fp, starting in col1,
 *		and followed by right-justified comment.
 * --------------------------------------------------------------------------
 * Arguments:	fp (I)		File ptr to output device (defaults to
 *				stdout if passed as NULL).
 *		col1 (I)	int containing 0 or #blanks preceding string
 *		string (I)	char *  containing string to be emitted.
 *				If last char of string is '\n',
 *				the emitted line ends with a newline,
 *				otherwise not.
 *		comment (I)	NULL or char * containing right-justified
 *				comment (we enclose between /star and star/)
 * --------------------------------------------------------------------------
 * Returns:	( int )		1 if completed successfully,
 *				or 0 otherwise (for any error).
 * --------------------------------------------------------------------------
 * Notes:     o
 * ======================================================================= */
/* --- entry point --- */
int	emit_string ( FILE *fp, int col1, char *string, char *comment )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
char	line[256];		/* construct line with caller's fields */
int	fieldlen;		/* #chars in one of caller's fields */
int	linelen = 72;		/*line length (for right-justified comment)*/
int	isnewline = 0;		/* true to emit \n at end of line */
/* --------------------------------------------------------------------------
construct line containing prolog, string, epilog, and finally comment
-------------------------------------------------------------------------- */
/* --- init line --- */
memset(line,' ',255);			/* start line with blanks */
/* --- embed string into line --- */
if ( string != NULL )			/* if caller gave us a string... */
  { fieldlen = strlen(string);		/* #cols required for string */
    if ( string[fieldlen-1] == '\n' )	/* check last char for newline */
      {	isnewline = 1;			/* got it, so set flag */
	fieldlen--; }			/* but don't print it yet */
    memcpy(line+col1,string,fieldlen);	/* embid string starting at col1 */
    col1 += fieldlen; }			/* bump col past epilog */
/* --- embed comment into line --- */
if ( comment != NULL )			/* if caller gave us a comment... */
  { fieldlen = 6 + strlen(comment);	/* plus  /star, star/, 2 spaces */
    if ( linelen-fieldlen < col1 )	/* comment won't fit */
      fieldlen -= (col1 - (linelen-fieldlen)); /* truncate comment to fit */
    if ( fieldlen > 6 )			/* can fit all or part of comment */
      sprintf(line+linelen-fieldlen,"/%c %.*s %c/", /* so embed it in line */
	'*', fieldlen-6,comment, '*');
    col1 = linelen; }			/* indicate line filled */
/* --- line completed --- */
line[col1] = '\000';			/* null-terminate completed line */
/* -------------------------------------------------------------------------
emit line, then back to caller with 1=okay, 0=failed.
-------------------------------------------------------------------------- */
/* --- first redirect null fp --- */
if ( fp == (FILE *)NULL ) fp = stdout;	/* default fp to stdout if null */
/* --- emit line (and optional newline) --- */
fprintf(fp,"%.*s",linelen,line);	/* no more than linelen chars */
if ( isnewline ) fprintf(fp,"\n");	/*caller wants terminating newline*/
return ( 1 );
} /* --- end-of-function emit_string() --- */


/* ==========================================================================
 * Function:	gftobitmap ( gf )
 * Purpose:	convert .gf-like pixmap to bitmap image
 * --------------------------------------------------------------------------
 * Arguments:	gf (I)		raster * to struct in .gf-format
 * --------------------------------------------------------------------------
 * Returns:	( raster * )	image-format raster * if successful,
 *				or NULL for any error.
 * --------------------------------------------------------------------------
 * Notes:     o
 * ======================================================================= */
/* --- entry point --- */
raster	*gftobitmap ( raster *gf )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
raster	*new_raster(), *rp=NULL;	/* image raster retuned to caller */
int	width=0, height=0, totbits=0;	/* gf->width, gf->height, #bits */
int	format=0, icount=0, ncounts=0,	/*.gf format, count index, #counts*/
	ibit=0, bitval=0;		/* bitmap index, bit value */
int	isrepeat = 1,			/* true to process repeat counts */
	repeatcmds[2] = {255,15},	/*opcode for repeat/duplicate count*/
	nrepeats=0, irepeat=0,		/* scan line repeat count,index */
	wbits = 0;			/* count bits to width of scan line*/
/* -------------------------------------------------------------------------
initialization
-------------------------------------------------------------------------- */
/* --- check args --- */
if ( gf == NULL ) goto end_of_job;	/* input raster not provided */
format = gf->format;			/* 2 or 3 */
if ( format!=2 && format!=3 ) goto end_of_job; /* invalid raster format */
ncounts = gf->pixsz;			/*pixsz is really #counts in pixmap*/
/* --- allocate output raster with proper dimensions for bitmap --- */
width=gf->width;  height=gf->height;	/* dimensions of raster */
if ( (rp = new_raster(width,height,1))	/* allocate new raster and bitmap */
==   NULL ) goto end_of_job;		/* quit if failed to allocate */
totbits = width*height;			/* total #bits in image */
/* -------------------------------------------------------------------------
fill bitmap
-------------------------------------------------------------------------- */
for ( icount=0,bitval=0; icount<ncounts; icount++ )
  {
  int	nbits = (int)(getbyfmt(format,gf->pixmap,icount)); /*#bits to set*/
  if ( isrepeat				/* we're proxessing repeat counts */
  &&   nbits == repeatcmds[format-2] ) { /* and repeat opcode found */
   if ( nrepeats == 0 )			/* recursive repeat is error */
    { nrepeats = (int)(getbyfmt(format,gf->pixmap,icount+1));/*repeat count*/
      nbits = (int)(getbyfmt(format,gf->pixmap,icount+2)); /*#bits to set*/
      icount += 2; }			/* bump byte/nibble count */
   else					/* some internal error occurred */
    if ( msgfp!=NULL && msglevel>=1 )	/* report error */
     fprintf(msgfp,"gftobitmap> found embedded repeat command\n"); }
  if ( 0 )
    fprintf(stdout,
    "gftobitmap> icount=%d bitval=%d nbits=%d ibit=%d totbits=%d\n",
    icount,bitval,nbits,ibit,totbits);
  for ( ; nbits>0; nbits-- )		/* count down */
    { if ( ibit >= totbits ) goto end_of_job; /* overflow check */
      for ( irepeat=0; irepeat<=nrepeats; irepeat++ )
       if ( bitval == 1 )		/* set pixel */
	{ setlongbit(rp->pixmap,(ibit+irepeat*width)); }
       else				/* clear pixel */
	{ unsetlongbit(rp->pixmap,(ibit+irepeat*width)); }
      if ( nrepeats > 0 ) wbits++;	/* count another repeated bit */
      ibit++; }				/* bump bit index */
  bitval = 1-bitval;			/* flip bit value */
  if ( wbits >= width ) {		/* completed repeats */
   ibit += nrepeats*width;		/*bump bit count past repeated scans*/
   if ( wbits > width )			/* out-of alignment error */
    if ( msgfp!=NULL && msglevel>=1 )	/* report error */
     fprintf(msgfp,"gftobitmap> width=%d wbits=%d\n",width,wbits);
   wbits = nrepeats = 0; }		/* reset repeat counts */
  } /* --- end-of-for(icount) --- */
end_of_job:
  return ( rp );			/* back to caller with image */
} /* --- end-of-function gftobitmap() --- */


/* ==========================================================================
 * Function:	get_symdef ( symbol )
 * Purpose:	returns mathchardef struct for symbol
 * --------------------------------------------------------------------------
 * Arguments:	symbol (I)	char *  containing symbol
 *				whose corresponding mathchardef is wanted
 * --------------------------------------------------------------------------
 * Returns:	( mathchardef * )  pointer to struct defining symbol,
 *				or NULL for any error
 * --------------------------------------------------------------------------
 * Notes:     o	Input symbol need only contain a leading substring to match,
 *		e.g., \gam passed in symbol will match \gamma in the table.
 *		If the table contains two or more possible matches,
 *		the shortest is returned, e.g., input \e will return with
 *		data for \eta rather than \epsilon.  To get \epsilon,
 *		you must pass a leading substring long enough to eliminate
 *		shorter table matches, i.e., in this case \ep
 * ======================================================================= */
/* --- entry point --- */
mathchardef *get_symdef ( char *symbol )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
mathchardef *symdefs = symtable;	/* table of mathchardefs */
int	ligdef=0, get_ligature();	/* or we may have a ligature */
int	idef = 0,			/* symdefs[] index */
	bestdef = (-9999);		/*index of shortest matching symdef*/
int	symlen = strlen(symbol),	/* length of input symbol */
	deflen, minlen=9999;		/*length of shortest matching symdef*/
int	/*alnumsym = (symlen==1 && isalnum(*symbol)),*/ /*alphanumeric sym*/
	alphasym = (symlen==1 && isalpha(*symbol)), /* or alpha symbol */
	slashsym = (*symbol=='\\');	/* or \backslashed symbol */
int	family = fontinfo[fontnum].family; /* current font family */
static	char *displaysyms[][2] = {	/*xlate to Big sym for \displaystyle*/
	/* --- see table on page 536 in TLC2 --- */
	{"\\int",	"\\Bigint"},
	{"\\oint",	"\\Bigoint"},
	{"\\sum",	"\\Bigsum"},
	{"\\prod",	"\\Bigprod"},
	{"\\coprod",	"\\Bigcoprod"},
	/* --- must be 'big' when related to similar binary operators --- */
	{"\\bigcup",	"\\Bigcup"},
	{"\\bigsqcup",	"\\Bigsqcup"},
	{"\\bigcap",	"\\Bigcap"},
	/*{"\\bigsqcap", "\\sqcap"},*/	/* don't have \Bigsqcap */
	{"\\bigodot",	"\\Bigodot"},
	{"\\bigoplus",	"\\Bigoplus"},
	{"\\bigominus",	"\\ominus"},
	{"\\bigotimes",	"\\Bigotimes"},
	{"\\bigoslash",	"\\oslash"},
	{"\\biguplus",	"\\Biguplus"},
	{"\\bigwedge",	"\\Bigwedge"},
	{"\\bigvee",	"\\Bigvee"},
	{NULL, NULL} };
/* -------------------------------------------------------------------------
First check for ligature
-------------------------------------------------------------------------- */
isligature = 0;				/* init signal for no ligature */
if ( family == CYR10 )			/*only check for cyrillic ligatures*/
 if ( (ligdef=get_ligature(subexprptr,family)) /* check for ligature */
 >=    0  )				/* found a ligature */
  { bestdef = ligdef;			/* set bestdef for ligature */
    isligature = 1;			/* signal we found a ligature */
    goto end_of_job; }			/* so just give it to caller */
/* -------------------------------------------------------------------------
If in \displaystyle mode, first xlate int to Bigint, etc.
-------------------------------------------------------------------------- */
if ( isdisplaystyle > 1 )		/* we're in \displaystyle mode */
  for ( idef=0; ; idef++ ) {		/* lookup symbol in displaysyms */
    char *fromsym = displaysyms[idef][0], /* look for this symbol */
	 *tosym = displaysyms[idef][1];	  /* and xlate it to this symbol */
    if ( fromsym == NULL ) break;	/* end-of-table */
    if ( !strcmp(symbol,fromsym) )	/* found a match */
      {	if ( msglevel>=99 && msgfp!=NULL ) /* debugging output */
	 { fprintf(msgfp,"get_symdef> isdisplaystyle=%d, xlated %s to %s\n",
	   isdisplaystyle,symbol,tosym); fflush(msgfp); }
	symbol = tosym;			/* so look up tosym instead */
	symlen = strlen(symbol);	/* reset symbol length */
	break; }			/* no need to search further */
    } /* --- end-of-for(idef) --- */
/* -------------------------------------------------------------------------
search symdefs[] in order for first occurrence of symbol
-------------------------------------------------------------------------- */
for ( idef=0; ;idef++ )			/* until trailer record found */
  if ( symdefs[idef].symbol == NULL ) break; /* reached end-of-table */
  else					/* check against caller's symbol */
    if ( strncmp(symbol,symdefs[idef].symbol,symlen) == 0 ) /* found match */
     if ( (fontnum==0||family==CYR10)	/* mathmode, so check every match */
     || (1 && symdefs[idef].handler!=NULL) /* or check every directive */
     || (1 && istextmode && slashsym)	/*text mode and \backslashed symbol*/
     || (0 && istextmode && (!alphasym	/* text mode and not alpha symbol */
	|| symdefs[idef].handler!=NULL))   /* or text mode and directive */
     || (symdefs[idef].family==family	/* have correct family */
	&& symdefs[idef].handler==NULL) )  /* and not a handler collision */
#if 0
     || (fontnum==1 && symdefs[idef].family==CMR10)   /*textmode && rm text*/
     || (fontnum==2 && symdefs[idef].family==CMMI10)  /*textmode && it text*/
     || (fontnum==3 && symdefs[idef].family==BBOLD10  /*textmode && bb text*/
	&& symdefs[idef].handler==NULL)
     || (fontnum==4 && symdefs[idef].family==CMMIB10  /*textmode && bf text*/
	&& symdefs[idef].handler==NULL) )
#endif
      if ( (deflen=strlen(symdefs[idef].symbol)) < minlen ) /*new best match*/
	{ bestdef = idef;		/* save index of new best match */
	  if ( (minlen = deflen)	/* and save its len for next test */
	  ==  symlen ) break; }		/*perfect match, so return with it*/
if ( bestdef < 0 )			/* failed to look up symbol */
  if ( fontnum != 0 )			/* we're in a restricted font mode */
    { int oldfontnum = fontnum;		/* save current font family */
      mathchardef *symdef = NULL;	/* lookup result with fontnum=0 */
      fontnum = 0;			/*try to look up symbol in any font*/
      symdef = get_symdef(symbol);	/* repeat lookup with fontnum=0 */
      fontnum = oldfontnum;		/* reset font family */
      return symdef; }			/* caller gets fontnum=0 lookup */
end_of_job:
 if ( msgfp!=NULL && msglevel>=999 )	/* debugging output */
  { fprintf(msgfp,
    "get_symdef> symbol=%s matches symtable[%d]=%s (isligature=%d)\n",
    symbol,bestdef,(bestdef<0?"NotFound":symdefs[bestdef].symbol),isligature);
    fflush(msgfp); }
 return ( (bestdef<0? NULL : &(symdefs[bestdef])) );/*NULL or best symdef[]*/
} /* --- end-of-function get_symdef() --- */


/* ==========================================================================
 * Function:	get_ligature ( expression, family )
 * Purpose:	returns symtable[] index for ligature
 * --------------------------------------------------------------------------
 * Arguments:	expression (I)	char *  containing ligature
 *				whose corresponding mathchardef is wanted
 *		family (I)	int containing NOVALUE for any family,
 *				or, e.g., CYR10 for cyrillic, etc.
 * --------------------------------------------------------------------------
 * Returns:	( int ) 	symtable[] index defining ligature,
 *				or -9999 if no ligature found or for any error
 * --------------------------------------------------------------------------
 * Notes:     o
 * ======================================================================= */
/* --- entry point --- */
int	get_ligature ( char *expression, int family )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
mathchardef *symdefs = symtable;	/* table of mathchardefs */
char	*ligature = expression /*- 1*/,	/* expression ptr */
	*symbol = NULL;			/* symdefs[idef].symbol */
int	liglen = strlen(ligature);	/* #chars remaining in expression */
int	iscyrfam = (family==CYR10);	/* true for cyrillic families */
int	idef = 0,			/* symdefs[] index */
	bestdef = (-9999),		/*index of longest matching symdef*/
	maxlen=(-9999);			/*length of longest matching symdef*/
/* -------------------------------------------------------------------------
search symdefs[] in order for first occurrence of symbol
-------------------------------------------------------------------------- */
if ( !isstring ) {			/* no ligatures in "string" mode */
 for ( idef=0; ;idef++ )		/* until trailer record found */
  if ( (symbol=symdefs[idef].symbol) == NULL ) break; /* end-of-table */
  else {				/* check against caller's ligature */
    int symlen = strlen(symbol);	/* #chars in symbol */
    if ( ( symlen>1 || iscyrfam )	/*ligature >1 char long or cyrillic*/
    &&   symlen <= liglen		/* and enough remaining chars */
    &&   ( *symbol!='\\' || iscyrfam )	/* not escaped or cyrillic */
    &&   symdefs[idef].handler == NULL ) /* and not a handler */
     if ( strncmp(ligature,symbol,symlen) == 0 ) /* found match */
      if ( family < 0			/* no family specifies */
      ||   symdefs[idef].family == family ) /* or have correct family */
       if ( symlen > maxlen )		/* new longest ligature */
	{ bestdef = idef;		/* save index of new best match */
	  maxlen = symlen; }		/* and save its len for next test */
    } /* --- end-of-if/else(symbol==NULL) --- */
 if ( msgfp!=NULL && msglevel>=999 )	/* debugging output */
  { fprintf(msgfp,"get_ligature> ligature=%.4s matches symtable[%d]=%s\n",
    ligature,bestdef,(bestdef<0?"NotFound":symdefs[bestdef].symbol));
    fflush(msgfp); }
 } /* --- end-of-if(!isstring) --- */
return ( bestdef );			/* -9999 or index of best symdef[] */
} /* --- end-of-function get_ligature --- */


/* ==========================================================================
 * Function:	get_chardef ( symdef, size )
 * Purpose:	returns chardef ptr containing data for symdef at given size
 * --------------------------------------------------------------------------
 * Arguments:	symdef (I)	mathchardef *  corresponding to symbol
 *				whose corresponding chardef is wanted
 *		size (I)	int containing 0-5 for desired size
 * --------------------------------------------------------------------------
 * Returns:	( chardef * )	pointer to struct defining symbol at size,
 *				or NULL for any error
 * --------------------------------------------------------------------------
 * Notes:     o	if size unavailable, the next-closer-to-normalsize
 *		is returned instead.
 * ======================================================================= */
/* --- entry point --- */
chardef	*get_chardef ( mathchardef *symdef, int size )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
fontfamily  *fonts = fonttable;		/* table of font families */
chardef	**fontdef,			/*tables for desired font, by size*/
	*gfdata = (chardef *)NULL;	/* chardef for symdef,size */
int	ifont;				/* fonts[] index */
int	family, charnum;		/* indexes retrieved from symdef */
int	sizeinc = 0,			/*+1 or -1 to get closer to normal*/
	normalsize = 2;			/* this size always present */
int	isBig = 0;			/*true if symbol's 1st char is upper*/
char	*symptr = NULL;			/* look for 1st alpha of symbol */
/* -------------------------------------------------------------------------
initialization
-------------------------------------------------------------------------- */
/* --- check symdef --- */
if ( symdef == NULL ) goto end_of_job;	/* get_symdef() probably failed */
/* --- get local copy of indexes from symdef --- */
family = symdef->family;		/* font family containing symbol */
charnum = symdef->charnum;		/* char# of symbol within font */
/* --- check for supersampling --- */
if ( issupersampling )			/* check for supersampling fonts */
 if ( fonts != ssfonttable )		/* uh oh--probably internal error */
  { fonts = ssfonttable; }		/* force it */
/* --- check requested size, and set size increment --- */
if ( 0 && issupersampling )		/* set size index for supersampling */
  size = LARGESTSIZE+1;			/* index 1 past largest size */
else					/* low pass indexes 0...LARGESTSIZE */
  {
  if( size<0 ) size = 0;		/* size was definitely too small */
  if( size>LARGESTSIZE ) size = LARGESTSIZE;  /* or definitely too large */
  if( size<normalsize ) sizeinc = (+1);	/*use next larger if size too small*/
  if( size>normalsize ) sizeinc = (-1);	/*or next smaller if size too large*/
  }
/* --- check for really big symbol (1st char of symbol name uppercase) --- */
for ( symptr=symdef->symbol; *symptr!='\000'; symptr++ ) /*skip leading \'s*/
  if ( isalpha(*symptr) )		/* found leading alpha char */
    { isBig = isupper(*symptr);		/* is 1st char of name uppercase? */
      if ( !isBig			/* 1st char lowercase */
      &&   strlen(symptr) >= 4 )	/* but followed by at least 3 chars */
       isBig = !memcmp(symptr,"big\\",4) /* isBig if name starts with big\ */
	|| !memcmp(symptr,"bigg",4);	/* or with bigg */
      break; }				/* don't check beyond 1st char */
/* -------------------------------------------------------------------------
find font family in table of fonts[]
-------------------------------------------------------------------------- */
/* --- look up font family --- */
for ( ifont=0; ;ifont++ )		/* until trailer record found */
  if ( fonts[ifont].family < 0 ) {	/* error, no such family */
    if ( msgfp!=NULL && msglevel>=99 ) { /* emit error */
     fprintf(msgfp,"get_chardef> failed to find font family %d\n",
     family); fflush(msgfp); }
    goto end_of_job; }			/* quit if can't find font family*/
  else if ( fonts[ifont].family == family ) break; /* found font family */
/* --- get local copy of table for this family by size --- */
fontdef = fonts[ifont].fontdef;		/* font by size */
/* -------------------------------------------------------------------------
get font in desired size, or closest available size, and return symbol
-------------------------------------------------------------------------- */
/* --- get font in desired size --- */
while ( 1 )				/* find size or closest available */
  if ( fontdef[size] != NULL ) break;	/* found available size */
  else					/* adjust size closer to normal */
    if ( size == NORMALSIZE		/* already normal so no more sizes,*/
    || sizeinc == 0 ) {			/* or must be supersampling */
      if ( msgfp!=NULL && msglevel>=99 ) { /* emit error */
	fprintf(msgfp,"get_chardef> failed to find font size %d\n",
	size); fflush(msgfp); }
      goto end_of_job; }		/* quit if can't find desired size */
    else				/*bump size 1 closer to NORMALSIZE*/
      size += sizeinc;			/* see if adjusted size available */
/* --- ptr to chardef struct --- */
gfdata = &((fontdef[size])[charnum]);	/*ptr to chardef for symbol in size*/
/* -------------------------------------------------------------------------
kludge to tweak CMEX10 (which appears to have incorrect descenders)
-------------------------------------------------------------------------- */
if ( family == CMEX10 )			/* cmex10 needs tweak */
  { int height = gfdata->toprow - gfdata->botrow + 1; /*total height of char*/
    gfdata->botrow = (isBig? (-height/3) : (-height/4));
    gfdata->toprow = gfdata->botrow + gfdata->image.height; }
/* -------------------------------------------------------------------------
return subraster containing chardef data for symbol in requested size
-------------------------------------------------------------------------- */
end_of_job:
 if ( msgfp!=NULL && msglevel>=999 )
  { if (symdef == NULL) fprintf(msgfp,"get_chardef> input symdef==NULL\n");
    else
     fprintf(msgfp,"get_chardef> requested symbol=\"%s\" size=%d  %s\n",
     symdef->symbol,size,(gfdata==NULL?"FAILED":"Succeeded"));
    fflush(msgfp); }
 return ( gfdata );			/*ptr to chardef for symbol in size*/
} /* --- end-of-function get_chardef() --- */


/* ==========================================================================
 * Function:	get_charsubraster ( symdef, size )
 * Purpose:	returns new subraster ptr containing
 *		data for symdef at given size
 * --------------------------------------------------------------------------
 * Arguments:	symdef (I)	mathchardef *  corresponding to symbol whose
 *				corresponding chardef subraster is wanted
 *		size (I)	int containing 0-5 for desired size
 * --------------------------------------------------------------------------
 * Returns:	( subraster * )	pointer to struct defining symbol at size,
 *				or NULL for any error
 * --------------------------------------------------------------------------
 * Notes:     o	just wraps a subraster envelope around get_chardef()
 * ======================================================================= */
/* --- entry point --- */
subraster *get_charsubraster ( mathchardef *symdef, int size )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
chardef	*get_chardef(), *gfdata=NULL;	/* chardef struct for symdef,size */
int	get_baseline();			/* baseline of gfdata */
subraster *new_subraster(), *sp=NULL;	/* subraster containing gfdata */
raster	*bitmaprp=NULL, *gftobitmap();	/* convert .gf-format to bitmap */
raster	*rotp=NULL, *rastrot3d();	/* rotate character if utheta>0 */
int	delete_subraster();		/* in case gftobitmap() fails */
int	delete_raster();		/* in case IMAGERASTER replaced */
int	aasupsamp(),			/*antialias char with supersampling*/
	grayscale=256;			/* aasupersamp() parameters */
/* -------------------------------------------------------------------------
look up chardef for symdef at size, and embed data (gfdata) in subraster
-------------------------------------------------------------------------- */
if ( (gfdata=get_chardef(symdef,size))	/* look up chardef for symdef,size */
!=   NULL )				/* and check that we found it */
 if ( (sp=new_subraster(0,0,0))		/* allocate subraster "envelope" */
 !=   NULL )				/* and check that we succeeded */
  {
  raster *image = &(gfdata->image);	/* ptr to chardef's bitmap or .gf */
  int format = image->format;		/* 1=bitmap, else .gf */
  sp->symdef = symdef;			/* replace NULL with caller's arg */
  sp->size = size;			/*replace default with caller's size*/
  sp->baseline = get_baseline(gfdata);	/* get baseline of character */
  if ( format == 1 )			/* already a bitmap */
   { sp->type = CHARASTER;		/* static char raster */
     sp->image = image; }		/* store ptr to its bitmap */
  else					/* need to convert .gf-to-bitmap */
   if ( (bitmaprp = gftobitmap(image))	/* convert */
   !=   (raster *)NULL )		/* successful */
    { sp->type = IMAGERASTER;		/* allocated raster will be freed */
      sp->image = bitmaprp; }		/* store ptr to converted bitmap */
   else					/* conversion failed */
    { delete_subraster(sp);		/* free unneeded subraster */
      sp = (subraster *)NULL;		/* signal error to caller */
      goto end_of_job; }		/* quit */
  if ( issupersampling )		/* antialias character right here */
    {
    raster *aa = NULL;			/* antialiased char raster */
    int status = aasupsamp(sp->image,&aa,shrinkfactor,grayscale);
    if ( status )			/* supersampled successfully */
      {	int baseline = sp->baseline;	/* baseline before supersampling */
	int height = gfdata->image.height; /* #rows before supersampling */
	sp->image = aa;			/* replace chardef with ss image */
	if ( baseline >= height-1 )	/* baseline at bottom of char */
	  sp->baseline = aa->height -1;	/* so keep it at bottom */
	else				/* char has descenders */
	  sp->baseline /= shrinkfactor;	/* rescale baseline */
	sp->type = IMAGERASTER; }	/* character is an image raster */
    } /* --- end-of-if(issupersampling) --- */
  if ( absval(utheta) > 1.0e-6 ) {	/* character 3d-rotation wanted */
    rotp = rastrot3d(sp->image,&uaxis,utheta); /* rotate image around uaxis */
    if ( sp->type != CHARASTER )	/* not a static character raster */
      delete_raster(sp->image);		/* so free currently allocated image */
    sp->image = rotp;			/* and replace it with rotated image */
    sp->type = IMAGERASTER;		/* allocated raster will be freed */
    } /* --- end-of-if(absval(utheta)>1.0e-6) --- */
  } /* --- end-of-if(sp!=NULL) --- */
end_of_job:
 if ( msgfp!=NULL && msglevel>=999 )
  { fprintf(msgfp,"get_charsubraster> requested symbol=\"%s\" baseline=%d"
    " %s %s\n", symdef->symbol, (sp==NULL?0:sp->baseline),
    (sp==NULL?"FAILED":"Succeeded"), (gfdata==NULL?"(gfdata=NULL)":" "));
    fflush(msgfp); }
return ( sp );				/* back to caller */
} /* --- end-of-function get_charsubraster() --- */


/* ==========================================================================
 * Function:	get_symsubraster ( symbol, size )
 * Purpose:	returns new subraster ptr containing
 *		data for symbol at given size
 * --------------------------------------------------------------------------
 * Arguments:	symbol (I)	char *  corresponding to symbol
 *				whose corresponding subraster is wanted
 *		size (I)	int containing 0-5 for desired size
 * --------------------------------------------------------------------------
 * Returns:	( subraster * )	pointer to struct defining symbol at size,
 *				or NULL for any error
 * --------------------------------------------------------------------------
 * Notes:     o	just combines get_symdef() and get_charsubraster()
 * ======================================================================= */
/* --- entry point --- */
subraster *get_symsubraster ( char *symbol, int size )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
subraster *sp=NULL, *get_charsubraster(); /* subraster containing gfdata */
mathchardef *symdef=NULL, *get_symdef(); /* mathchardef lookup for symbol */
/* -------------------------------------------------------------------------
look up mathchardef for symbol
-------------------------------------------------------------------------- */
if ( symbol != NULL )			/* user supplied input symbol */
  symdef = get_symdef(symbol);		/*look up corresponding mathchardef*/
/* -------------------------------------------------------------------------
look up chardef for mathchardef and wrap a subraster structure around data
-------------------------------------------------------------------------- */
if ( symdef != NULL )			/* lookup succeeded */
  sp = get_charsubraster(symdef,size);	/* so get symbol data in subraster */
return ( sp );				/* back to caller with sp or NULL */
} /* --- end-of-function get_symsubraster() --- */


/* ==========================================================================
 * Function:	get_baseline ( gfdata )
 * Purpose:	returns baseline for a chardef struct
 * --------------------------------------------------------------------------
 * Arguments:	gfdata (I)	chardef *  containing chardef for symbol
 *				whose baseline is wanted
 * --------------------------------------------------------------------------
 * Returns:	( int )		baseline for symdef,
 *				or -1 for any error
 * --------------------------------------------------------------------------
 * Notes:     o	Unlike TeX, the top-left corners of our rasters are (0,0),
 *		with (row,col) increasing as you move down and right.
 *		Baselines are calculated with respect to this scheme,
 *		so 0 would mean the very top row is on the baseline
 *		and everything else descends below the baseline.
 * ======================================================================= */
/* --- entry point --- */
int	get_baseline ( chardef *gfdata )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
int	/*toprow = gfdata->toprow,*/	/*TeX top row from .gf file info*/
	botrow = gfdata->botrow,	/*TeX bottom row from .gf file info*/
	height = gfdata->image.height;	/* #rows comprising symbol */
/* -------------------------------------------------------------------------
give caller baseline
-------------------------------------------------------------------------- */
return ( (height-1) + botrow );		/* note: descenders have botrow<0 */
} /* --- end-of-function get_baseline() --- */


/* ==========================================================================
 * Function:	get_delim ( char *symbol, int height, int family )
 * Purpose:	returns subraster corresponding to the samllest
 *		character containing symbol, but at least as large as height,
 *		and in caller's family (if specified).
 *		If no symbol character as large as height is available,
 *		then the largest availabale character is returned instead.
 * --------------------------------------------------------------------------
 * Arguments:	symbol (I)	char *  containing (substring of) desired
 *				symbol, e.g., if symbol="(", then any
 *				mathchardef like "(" or "\\(", etc, match.
 *		height (I)	int containing minimum acceptable height
 *				for returned character
 *		family (I)	int containing -1 to consider all families,
 *				or, e.g., CMEX10 for only that family
 * --------------------------------------------------------------------------
 * Returns:	( subraster * )	best matching character available,
 *				or NULL for any error
 * --------------------------------------------------------------------------
 * Notes:     o	If height is passed as negative, its absolute value is used
 *		but the best-fit width is searched for (rather than height)
 * ======================================================================= */
/* --- entry point --- */
subraster *get_delim ( char *symbol, int height, int family )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
mathchardef *symdefs = symtable;	/* table of mathchardefs */
subraster *get_charsubraster(), *sp=(subraster *)NULL; /* best match char */
subraster *make_delim();		/* construct delim if can't find it*/
chardef	*get_chardef(), *gfdata=NULL;	/* get chardef struct for a symdef */
char	lcsymbol[256], *symptr,		/* lowercase symbol for comparison */
	*unescsymbol = symbol;		/* unescaped symbol */
int	symlen = (symbol==NULL?0:strlen(symbol)), /* #chars in caller's sym*/
	deflen = 0;			/* length of symdef (aka lcsymbol) */
int	idef = 0,			/* symdefs[] index */
	bestdef = (-9999),		/* index of best fit symdef */
	bigdef = (-9999);		/*index of biggest (in case no best)*/
int	size = 0,			/* size index 0...LARGESTSIZE */
	bestsize = (-9999),		/* index of best fit size */
	bigsize = (-9999);		/*index of biggest (in case no best)*/
int	defheight, bestheight=9999,	/* height of best fit symdef */
	bigheight = (-9999);		/*height of biggest(in case no best)*/
int	iswidth = 0;			/* true if best-fit width desired */
int	isunesc = 0,			/* true if leading escape removed */
	issq=0, isoint=0;		/* true for \sqcup,etc, \oint,etc */
int	iscurly = 0;			/* true for StMary's curly symbols */
char	*bigint="bigint", *bigoint="bigoint"; /* substitutes for int, oint */
/* -------------------------------------------------------------------------
determine if searching height or width, and search symdefs[] for best-fit
-------------------------------------------------------------------------- */
/* --- arg checks --- */
if ( symlen < 1 ) return (sp);		/* no input symbol suplied */
if ( strcmp(symbol,"e") == 0 ) return(sp); /* e causes segfault??? */
if ( strstr(symbol,"curly") != NULL ) iscurly=1; /* user wants curly delim */
/* --- ignore leading escapes for CMEX10 --- */
if ( 1 )				/* ignore leading escape */
 if ( (family==CMEX10 || family==CMSYEX) ) { /* for CMEX10 or CMSYEX */
  if ( strstr(symbol,"sq") != NULL )	/* \sq symbol requested */
     issq = 1;				/* seq \sq signal */
  if ( strstr(symbol,"oint") != NULL )	/* \oint symbol requested */
     isoint = 1;			/* seq \oint signal */
  if ( *symbol=='\\' )			/* have leading \ */
   { unescsymbol = symbol+1;		/* push past leading \ */
     if ( --symlen < 1 ) return(sp);	/* one less char */
     if ( strcmp(unescsymbol,"int") == 0 ) /* \int requested by caller */
       unescsymbol = bigint;		/* but big version looks better */
     if ( strcmp(unescsymbol,"oint") == 0 ) /* \oint requested by caller */
       unescsymbol = bigoint;		/* but big version looks better */
     symlen = strlen(unescsymbol);	/* explicitly recalculate length */
     isunesc = 1; }			/* signal leading escape removed */
  } /* --- end-of-if(family) --- */
/* --- determine whether searching for best-fit height or width --- */
if ( height < 0 )			/* negative signals width search */
  { height = (-height);			/* flip "height" positive */
    iswidth = 1; }			/* set flag for width search */
/* --- search symdefs[] for best-fit height (or width) --- */
for ( idef=0; ;idef++ )			/* until trailer record found */
 {
 char *defsym = symdefs[idef].symbol;	/* local copies */
 int  deffam  = symdefs[idef].family;
 if ( defsym == NULL ) break;		/* reached end-of-table */
 else					/* check against caller's symbol */
  if ( family<0 || deffam == family	/* if explicitly in caller's family*/
  ||  (family==CMSYEX && (deffam==CMSY10||deffam==CMEX10||deffam==STMARY10)) )
    {
    strcpy(lcsymbol,defsym);		/* local copy of symdefs[] symbol */
    if ( isunesc && *lcsymbol=='\\' )	/* ignored leading \ in symbol */
     {strsqueeze(lcsymbol,1);}		/*so squeeze it out of lcsymbol too*/
    if ( 0 )				/* don't ignore case */
     for ( symptr=lcsymbol; *symptr!='\000'; symptr++ )/*for each symbol ch*/
      if ( isalpha(*symptr) ) *symptr=tolower(*symptr);/*lowercase the char*/
    deflen = strlen(lcsymbol);		/* #chars in symbol we're checking */
    if ((symptr=strstr(lcsymbol,unescsymbol)) != NULL) /*found caller's sym*/
     if ( (isoint || strstr(lcsymbol,"oint")==NULL) /* skip unwanted "oint"*/
     &&   (issq || strstr(lcsymbol,"sq")==NULL) ) /* skip unwanted "sq" */
      if ( ( deffam == CMSY10 ?		/* CMSY10 or not CMSY10 */
	  symptr == lcsymbol		/* caller's sym is a prefix */
          && deflen == symlen:		/* and same length */
	  (iscurly || strstr(lcsymbol,"curly")==NULL) &&/*not unwanted curly*/
	  (symptr == lcsymbol		/* caller's sym is a prefix */
          || symptr == lcsymbol+deflen-symlen) ) ) /* or a suffix */
       for ( size=0; size<=LARGESTSIZE; size++ ) /* check all font sizes */
	if ( (gfdata=get_chardef(&(symdefs[idef]),size)) != NULL ) /*got one*/
	  { defheight = gfdata->image.height;	/* height of this character */
	    if ( iswidth )		/* width search wanted instead... */
	      defheight = gfdata->image.width;	/* ...so substitute width */
	    leftsymdef = &(symdefs[idef]);	/* set symbol class, etc */
	    if ( defheight>=height && defheight<bestheight ) /*new best fit*/
	      { bestdef=idef; bestsize=size;	/* save indexes of best fit */
		bestheight = defheight; }	/* and save new best height */
	    if ( defheight >= bigheight )	/* new biggest character */
	      { bigdef=idef; bigsize=size;	/* save indexes of biggest */
		bigheight = defheight; }	/* and save new big height */
          } /* --- end-of-if(gfdata!=NULL) --- */
    } /* --- end-of-if(family) --- */
 } /* --- end-of-for(idef) --- */
/* -------------------------------------------------------------------------
construct subraster for best fit character, and return it to caller
-------------------------------------------------------------------------- */
if ( bestdef >= 0 )			/* found a best fit for caller */
  sp = get_charsubraster(&(symdefs[bestdef]),bestsize); /* best subraster */
if ( (sp==NULL && height-bigheight>5)	/* try to construct delim */
||   bigdef < 0 )			/* delim not in font tables */
  sp = make_delim(symbol,(iswidth?-height:height)); /* try to build delim */
if ( sp==NULL && bigdef>=0 )		/* just give biggest to caller */
  sp = get_charsubraster(&(symdefs[bigdef]),bigsize); /* biggest subraster */
if ( msgfp!=NULL && msglevel>=99 )
    fprintf(msgfp,"get_delim> symbol=%.50s, height=%d family=%d isokay=%s\n",
    (symbol==NULL?"null":symbol),height,family,(sp==NULL?"fail":"success"));
return ( sp );
} /* --- end-of-function get_delim() --- */


/* ==========================================================================
 * Function:	make_delim ( char *symbol, int height )
 * Purpose:	constructs subraster corresponding to symbol
 *		exactly as large as height,
 * --------------------------------------------------------------------------
 * Arguments:	symbol (I)	char *  containing, e.g., if symbol="("
 *				for desired delimiter
 *		height (I)	int containing height
 *				for returned character
 * --------------------------------------------------------------------------
 * Returns:	( subraster * )	constructed delimiter
 *				or NULL for any error
 * --------------------------------------------------------------------------
 * Notes:     o	If height is passed as negative, its absolute value is used
 *		and interpreted as width (rather than height)
 * ======================================================================= */
/* --- entry point --- */
subraster *make_delim ( char *symbol, int height )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
subraster *sp = (subraster *)NULL,	/* subraster returned to caller */
	*new_subraster();		/* allocate subraster */
subraster *get_symsubraster(),		/* look up delim pieces in cmex10 */
	*symtop=NULL, *symbot=NULL, *symmid=NULL, *symbar=NULL,	/* pieces */
	*topsym=NULL, *botsym=NULL, *midsym=NULL, *barsym=NULL,	/* +filler */
	*rastack(), *rastcat();		/* stack pieces, concat filler */
int	isdrawparen = 0;		/*1=draw paren, 0=build from pieces*/
raster	*rasp = (raster *)NULL;		/* sp->image */
int	isokay=0, delete_subraster();	/* set true if delimiter drawn ok */
int	pixsz = 1,			/* pixels are one bit each */
	symsize = 0;			/* size arg for get_symsubraster() */
int	thickness = 1;			/* drawn lines are one pixel thick */
int	aspectratio = 8;		/* default height/width for parens */
int	iswidth = 0,			/*true if width specified by height*/
	width = height;			/* #pixels width (e.g., of ellipse)*/
char	*lp=NULL,  *rp=NULL,		/* check symbol for left or right */
	*lp2=NULL, *rp2=NULL,		/* synonym for lp,rp */
	*lp3=NULL, *rp3=NULL,		/* synonym for lp,rp */
	*lp4=NULL, *rp4=NULL;		/* synonym for lp,rp */
int	circle_raster(),		/* ellipse for ()'s in sp->image */
	rule_rsater(),			/* horizontal or vertical lines */
	line_raster();			/* line between two points */
subraster *uparrow_subraster();		/* up/down arrows */
int	isprealloc = 1;			/*pre-alloc subraster, except arrow*/
int	oldsmashmargin = smashmargin,	/* save original smashmargin */
	wasnocatspace = isnocatspace;	/* save original isnocatspace */
/* -------------------------------------------------------------------------
initialization
-------------------------------------------------------------------------- */
/* --- determine whether constructing height or width --- */
if ( height < 0 )			/* negative "height" signals width */
  { width = height = (-height);		/* flip height positive */
    iswidth = 1; }			/* set flag for width */
if ( height < 3 ) goto end_of_job;	/* too small, must be error */
/* --- set default width (or height) accordingly --- */
if ( iswidth ) height =  (width+(aspectratio+1)/2)/aspectratio;
else            width = (height+(aspectratio+1)/2)/aspectratio;
if ( strchr(symbol,'=') != NULL		/* left or right || bracket wanted */
||   strstr(symbol,"\\|") != NULL	/* same || in standard tex notation*/
||   strstr(symbol,"dbl") != NULL )	/* semantic bracket with ||'s */
  width = max2(width,6);		/* need space between two |'s */
if ( width < 2 ) width=2;		/* set min width */
if ( strchr(symbol,'(') != NULL		/* if left ( */
||   strchr(symbol,')') != NULL )	/* or right ) paren wanted */
  { width = (3*width)/2;		/* adjust width */
    if ( !isdrawparen ) isprealloc=0; }	/* don't prealloc if building */
if ( strchr(symbol,'/') != NULL		/* left / */
||   strstr(symbol,"\\\\") != NULL	/* or \\ for right \ */
||   strstr(symbol,"backsl") != NULL )	/* or \backslash for \ */
  width = max2(height/3,5);
if ( strstr(symbol,"arrow") != NULL )	/* arrow wanted */
  { width = min2(height/3,20);		/* adjust width */
    isprealloc = 0; }			/* don't preallocate subraster */
if ( strchr(symbol,'{') != NULL		/* if left { */
||   strchr(symbol,'}') != NULL )	/* or right } brace wanted */
  { isprealloc = 0; }			/* don't preallocate */
/* --- allocate and initialize subraster for constructed delimiter --- */
if ( isprealloc )			/* pre-allocation wanted */
 { if ( (sp=new_subraster(width,height,pixsz)) /* allocate new subraster */
   ==   NULL )  goto end_of_job;	/* quit if failed */
   /* --- initialize delimiter subraster parameters --- */
   sp->type = IMAGERASTER;		/* image */
   sp->symdef = NULL;			/* not applicable for image */
   sp->baseline = height/2 + 2;		/* is a little above center good? */
   sp->size = NORMALSIZE;		/* size (probably unneeded) */
   rasp = sp->image; }			/* pointer to image in subraster */
/* -------------------------------------------------------------------------
( ) parens
-------------------------------------------------------------------------- */
if ( (lp=strchr(symbol,'(')) != NULL	/* left ( paren wanted */
||   (rp=strchr(symbol,')')) != NULL )	/* right ) paren wanted */
  {
  if ( isdrawparen ) {			/* draw the paren */
   int	mywidth = min2(width,20);	/* max width for ()'s */
   circle_raster ( rasp,		/* embedded raster image */
	0, 0,				/* row0,col0 are upper-left corner */
	height-1, mywidth-1,		/* row1,col1 are lower-right */
	thickness,			/* line thickness is 1 pixel */
	(rp==NULL?"23":"41") );		/* "1234" quadrants to be drawn */
   isokay = 1; }			/* set flag */
  else {
   int	isleft = (lp!=NULL?1:0);	/* true for left, false for right */
   char	*parentop = (isleft?"\\leftparentop":"\\rightparentop"),
	*parenbot = (isleft?"\\leftparenbot":"\\rightparenbot"),
	*parenbar = (isleft?"\\leftparenbar":"\\rightparenbar");
   int	baseht=0, barht=0,		/* height of base=top+bot, bar */
	ibar=0, nbars=0;		/* bar index, #bars between top&bot*/
   int	largestsize = min2(2,LARGESTSIZE), /* largest size for parens */
	topfill=(isleft?0:0), botfill=(isleft?0:0),
	barfill=(isleft?0:7);		/* alignment fillers */
   /* --- get pieces at largest size smaller than total height --- */
   for ( symsize=largestsize; symsize>=0; symsize-- ) /*largest to smallest*/
    {
    /* --- get pieces at current test size --- */
    isokay = 1;				/* check for all pieces */
    if ( (symtop=get_symsubraster(parentop,symsize)) == NULL ) isokay=0;
    if ( (symbot=get_symsubraster(parenbot,symsize)) == NULL ) isokay=0;
    if ( (symbar=get_symsubraster(parenbar,symsize)) == NULL ) isokay=0;
    /* --- check sum of pieces against total desired height --- */
    if ( isokay ) {			/* all pieces retrieved */
      baseht = (symtop->image)->height + (symbot->image)->height; /*top+bot*/
      barht  = (symbar->image)->height;	/* bar height */
      if ( baseht < height+5 ) break;	/* largest base that's not too big */
      if ( symsize < 1 ) break;		/* or smallest available base */
      } /* --- end-of-if(isokay) --- */
    /* --- free test pieces that were too big --- */
    if ( symtop != NULL ) delete_subraster(symtop); /* free top */
    if ( symbot != NULL ) delete_subraster(symbot); /* free bot */
    if ( symbar != NULL ) delete_subraster(symbar); /* free bar */
    isokay = 0;				/* nothing available */
    if ( symsize < 1 ) break;		/* leave isokay=0 after smallest */
    } /* --- end-of-for(symsize) --- */
   /* --- construct brace from pieces --- */
   if ( isokay ) {			/* we have the pieces */
    /* --- add alignment fillers --- */
    smashmargin=0;  isnocatspace=99;	/*turn off rastcat smashing,space*/
    topsym = (topfill>0?rastcat(new_subraster(topfill,1,1),symtop,3):symtop);
    botsym = (botfill>0?rastcat(new_subraster(botfill,1,1),symbot,3):symbot);
    barsym = (barfill>0?rastcat(new_subraster(barfill,1,1),symbar,3):symbar);
    smashmargin = oldsmashmargin;	/* reset smashmargin */
    isnocatspace = wasnocatspace;	/* reset isnocatspace */
    /* --- #bars needed between top and bot --- */
    nbars = (barht<1?0:max2(0,1+(height-baseht)/barht)); /* #bars needed */
    /* --- stack pieces --- */
    sp = topsym;			/* start with top piece */
    if ( nbars > 0 )			/* need nbars between top and bot */
      for ( ibar=1; ibar<=nbars; ibar++ ) sp = rastack(barsym,sp,1,0,0,2);
    sp = rastack(botsym,sp,1,0,0,3);	/* bottom below bars or middle */
    delete_subraster(barsym);		/* barsym no longer needed */
    } /* --- end-of-if(isokay) --- */
   } /* --- end-of-if/else(isdrawparen) --- */
  } /* --- end-of-if(left- or right-() paren wanted) --- */
/* -------------------------------------------------------------------------
{ } braces
-------------------------------------------------------------------------- */
else
 if ( (lp=strchr(symbol,'{')) != NULL	/* left { brace wanted */
 ||   (rp=strchr(symbol,'}')) != NULL )	/* right } brace wanted */
  {
  int	isleft = (lp!=NULL?1:0);	/* true for left, false for right */
  char	*bracetop = (isleft?"\\leftbracetop":"\\rightbracetop"),
	*bracebot = (isleft?"\\leftbracebot":"\\rightbracebot"),
	*bracemid = (isleft?"\\leftbracemid":"\\rightbracemid"),
	*bracebar = (isleft?"\\leftbracebar":"\\rightbracebar");
  int	baseht=0, barht=0,		/* height of base=top+bot+mid, bar */
	ibar=0, nbars=0;		/* bar index, #bars above,below mid*/
  int	largestsize = min2(2,LARGESTSIZE), /* largest size for braces */
	topfill=(isleft?4:0), botfill=(isleft?4:0),
	midfill=(isleft?0:4), barfill=(isleft?4:4); /* alignment fillers */
  /* --- get pieces at largest size smaller than total height --- */
  for ( symsize=largestsize; symsize>=0; symsize-- ) /*largest to smallest*/
    {
    /* --- get pieces at current test size --- */
    isokay = 1;				/* check for all pieces */
    if ( (symtop=get_symsubraster(bracetop,symsize)) == NULL ) isokay=0;
    if ( (symbot=get_symsubraster(bracebot,symsize)) == NULL ) isokay=0;
    if ( (symmid=get_symsubraster(bracemid,symsize)) == NULL ) isokay=0;
    if ( (symbar=get_symsubraster(bracebar,symsize)) == NULL ) isokay=0;
    /* --- check sum of pieces against total desired height --- */
    if ( isokay ) {			/* all pieces retrieved */
      baseht = (symtop->image)->height + (symbot->image)->height
	+ (symmid->image)->height;	/* top+bot+mid height */
      barht = (symbar->image)->height;	/* bar height */
      if ( baseht < height+5 ) break;	/* largest base that's not too big */
      if ( symsize < 1 ) break;		/* or smallest available base */
      } /* --- end-of-if(isokay) --- */
    /* --- free test pieces that were too big --- */
    if ( symtop != NULL ) delete_subraster(symtop); /* free top */
    if ( symbot != NULL ) delete_subraster(symbot); /* free bot */
    if ( symmid != NULL ) delete_subraster(symmid); /* free mid */
    if ( symbar != NULL ) delete_subraster(symbar); /* free bar */
    isokay = 0;				/* nothing available */
    if ( symsize < 1 ) break;		/* leave isokay=0 after smallest */
    } /* --- end-of-for(symsize) --- */
  /* --- construct brace from pieces --- */
  if ( isokay ) {			/* we have the pieces */
    /* --- add alignment fillers --- */
    smashmargin=0;  isnocatspace=99;	/*turn off rastcat smashing,space*/
    topsym = (topfill>0?rastcat(new_subraster(topfill,1,1),symtop,3):symtop);
    botsym = (botfill>0?rastcat(new_subraster(botfill,1,1),symbot,3):symbot);
    midsym = (midfill>0?rastcat(new_subraster(midfill,1,1),symmid,3):symmid);
    barsym = (barfill>0?rastcat(new_subraster(barfill,1,1),symbar,3):symbar);
    smashmargin = oldsmashmargin;	/* reset smashmargin */
    isnocatspace = wasnocatspace;	/* reset isnocatspace */
    /* --- #bars needed on each side of mid piece --- */
    nbars = (barht<1?0:max2(0,1+(height-baseht)/barht/2)); /*#bars per side*/
    /* --- stack pieces --- */
    sp = topsym;			/* start with top piece */
    if ( nbars > 0 )			/* need nbars above middle */
      for ( ibar=1; ibar<=nbars; ibar++ ) sp = rastack(barsym,sp,1,0,0,2);
    sp = rastack(midsym,sp,1,0,0,3);	/*mid after top or bars*/
    if ( nbars > 0 )			/* need nbars below middle */
      for ( ibar=1; ibar<=nbars; ibar++ ) sp = rastack(barsym,sp,1,0,0,2);
    sp = rastack(botsym,sp,1,0,0,3);	/* bottom below bars or middle */
    delete_subraster(barsym);		/* barsym no longer needed */
    } /* --- end-of-if(isokay) --- */
  } /* --- end-of-if(left- or right-{} brace wanted) --- */
/* -------------------------------------------------------------------------
[ ] brackets
-------------------------------------------------------------------------- */
else
 if ( (lp=strchr(symbol,'[')) != NULL	/* left [ bracket wanted */
 ||   (rp=strchr(symbol,']')) != NULL	/* right ] bracket wanted */
 ||   (lp2=strstr(symbol,"lceil")) != NULL /* left ceiling wanted */
 ||   (rp2=strstr(symbol,"rceil")) != NULL /* right ceiling wanted */
 ||   (lp3=strstr(symbol,"lfloor")) != NULL /* left floor wanted */
 ||   (rp3=strstr(symbol,"rfloor")) != NULL /* right floor wanted */
 ||   (lp4=strstr(symbol,"llbrack")) != NULL /* left semantic bracket */
 ||   (rp4=strstr(symbol,"rrbrack")) != NULL ) /* right semantic bracket */
  {
  /* --- use rule_raster ( rasp, top, left, width, height, type=0 ) --- */
  int	mywidth = min2(width,12),	/* max width for horizontal bars */
	wthick = 1;			/* thickness of top.bottom bars */
  thickness = (height<25?1:2);		/* set lines 1 or 2 pixels thick */
  if ( lp2!=NULL || rp2!=NULL || lp3!=NULL || rp3 !=NULL ) /*ceil or floor*/
    wthick = thickness;			/* same thickness for top/bot bar */
  if ( lp3==NULL && rp3==NULL )		/* set top bar if floor not wanted */
    rule_raster(rasp, 0,0, mywidth,wthick, 0); /* top horizontal bar */
  if ( lp2==NULL && rp2==NULL )		/* set bot bar if ceil not wanted */
    rule_raster(rasp, height-wthick,0, mywidth,thickness, 0); /* bottom */
  if ( lp!=NULL || lp2!=NULL || lp3!=NULL || lp4!=NULL ) /* left bracket */
   rule_raster(rasp, 0,0, thickness,height, 0); /* left vertical bar */
  if ( lp4 != NULL )			/* 2nd left vertical bar needed */
   rule_raster(rasp, 0,thickness+1, 1,height, 0); /* 2nd left vertical bar */
  if ( rp!=NULL || rp2!=NULL || rp3!=NULL || rp4!=NULL ) /* right bracket */
   rule_raster(rasp, 0,mywidth-thickness, thickness,height, 0); /* right */
  if ( rp4 != NULL )			/* 2nd right vertical bar needed */
   rule_raster(rasp, 0,mywidth-thickness-2, 1,height, 0); /*2nd right vert*/
  isokay = 1;				/* set flag */
  } /* --- end-of-if(left- or right-[] bracket wanted) --- */
/* -------------------------------------------------------------------------
< > brackets
-------------------------------------------------------------------------- */
else
 if ( (lp=strchr(symbol,'<')) != NULL	/* left < bracket wanted */
 ||   (rp=strchr(symbol,'>')) != NULL )	/* right > bracket wanted */
  {
  /* --- use line_raster( rasp,  row0, col0,  row1, col1,  thickness ) --- */
  int	mywidth = min2(width,12),	/* max width for brackets */
	mythick = 1;			/* all lines one pixel thick */
  thickness = (height<25?1:2);		/* set line pixel thickness */
  if ( lp != NULL )			/* left < bracket wanted */
    { line_raster(rasp,height/2,0,0,mywidth-1,mythick);
      if ( thickness>1 )
	line_raster(rasp,height/2,1,0,mywidth-1,mythick);
      line_raster(rasp,height/2,0,height-1,mywidth-1,mythick);
      if ( thickness>1 )
	line_raster(rasp,height/2,1,height-1,mywidth-1,mythick); }
  if ( rp != NULL )			/* right > bracket wanted */
    { line_raster(rasp,height/2,mywidth-1,0,0,mythick);
      if ( thickness>1 )
	line_raster(rasp,height/2,mywidth-2,0,0,mythick);
      line_raster(rasp,height/2,mywidth-1,height-1,0,mythick);
      if ( thickness>1 )
	line_raster(rasp,height/2,mywidth-2,height-1,0,mythick); }
  isokay = 1;				/* set flag */
  } /* --- end-of-if(left- or right-<> bracket wanted) --- */
/* -------------------------------------------------------------------------
/ \ delimiters
-------------------------------------------------------------------------- */
else
 if ( (lp=strchr(symbol,'/')) != NULL	/* left /  wanted */
 ||   (rp=strstr(symbol,"\\\\")) != NULL /* right \ wanted */
 ||   (rp2=strstr(symbol,"backsl")) != NULL ) /* right \ wanted */
  {
  /* --- use line_raster( rasp,  row0, col0,  row1, col1,  thickness ) --- */
  int	mywidth = width;		/* max width for / \ */
  thickness = 1;			/* set line pixel thickness */
  if ( lp != NULL )			/* left / wanted */
    line_raster(rasp,0,mywidth-1,height-1,0,thickness);
  if ( rp!=NULL || rp2!=NULL )		/* right \ wanted */
    line_raster(rasp,0,0,height-1,mywidth-1,thickness);
  isokay = 1;				/* set flag */
  } /* --- end-of-if(left- or right-/\ delimiter wanted) --- */
/* -------------------------------------------------------------------------
arrow delimiters
-------------------------------------------------------------------------- */
else
 if ( strstr(symbol,"arrow") != NULL )	/* arrow delimiter wanted */
  {
  /* --- use uparrow_subraster(width,height,pixsz,drctn,isBig) --- */
  int	mywidth = width;		/* max width for / \ */
  int	isBig = (strstr(symbol,"Up")!=NULL /* isBig if we have an Up */
		|| strstr(symbol,"Down")!=NULL); /* or a Down */
  int	drctn = +1;			/* init for uparrow */
  if ( strstr(symbol,"down")!=NULL	/* down if we have down */
  ||   strstr(symbol,"Down")!=NULL )	/* or Down */
   { drctn = (-1);			/* reset direction to down */
     if ( strstr(symbol,"up")!=NULL	/* updown if we have up or Up */
     ||   strstr(symbol,"Up")!=NULL )	/* and down or Down */
      drctn = 0; }			/* reset direction to updown */
  sp = uparrow_subraster(mywidth,height,pixsz,drctn,isBig);
  if ( sp != NULL )
   { sp->type = IMAGERASTER;		/* image */
     sp->symdef = NULL;			/* not applicable for image */
     sp->baseline = height/2 + 2;	/* is a little above center good? */
     sp->size = NORMALSIZE;		/* size (probably unneeded) */
     isokay = 1; }			/* set flag */
  } /* --- end-of-if(arrow delimiter wanted) --- */
/* -------------------------------------------------------------------------
\- for | | brackets or \= for || || brackets
-------------------------------------------------------------------------- */
else
 if ( (lp=strchr(symbol,'-')) != NULL	/* left or right | bracket wanted */
 ||  (lp2=strchr(symbol,'|')) != NULL	/* synonym for | bracket */
 ||   (rp=strchr(symbol,'=')) != NULL	/* left or right || bracket wanted */
 || (rp2=strstr(symbol,"\\|"))!= NULL )	/* || in standard tex notation */
  {
  /* --- rule_raster ( rasp, top, left, width, height, type=0 ) --- */
  int	midcol = width/2;		/* middle col, left of mid if even */
  if ( rp  != NULL			/* left or right || bracket wanted */
  ||   rp2 != NULL )			/* or || in standard tex notation */
   { thickness = (height<75?1:2);	/* each | of || 1 or 2 pixels thick*/
     rule_raster(rasp, 0,max2(0,midcol-2), thickness,height, 0); /* left */
     rule_raster(rasp, 0,min2(width,midcol+2), thickness,height, 0); }
  else					/*nb, lp2 spuriously set if rp2 set*/
   if ( lp  != NULL			/* left or right | bracket wanted */
   ||   lp2 != NULL )			/* ditto for synomym */
    { thickness = (height<75?1:2);	/* set | 1 or 2 pixels thick */
      rule_raster(rasp, 0,midcol, thickness,height, 0); } /*mid vertical bar*/
  isokay = 1;				/* set flag */
  } /* --- end-of-if(left- or right-[] bracket wanted) --- */
/* -------------------------------------------------------------------------
back to caller
-------------------------------------------------------------------------- */
end_of_job:
  if ( msgfp!=NULL && msglevel>=99 )
    fprintf(msgfp,"make_delim> symbol=%.50s, isokay=%d\n",
    (symbol==NULL?"null":symbol),isokay);
  if ( !isokay )			/* don't have requested delimiter */
    { if (sp!=NULL) delete_subraster(sp); /* so free unneeded structure */
      sp = NULL; }			/* and signal error to caller */
  return ( sp );			/*back to caller with delim or NULL*/
} /* --- end-of-function make_delim() --- */


/* ==========================================================================
 * Function:	texchar ( expression, chartoken )
 * Purpose:	scans expression, returning either its first character,
 *		or the next \sequence if that first char is \,
 *		and a pointer to the first expression char past that.
 * --------------------------------------------------------------------------
 * Arguments:	expression (I)	char * to first char of null-terminated
 *				string containing valid LaTeX expression
 *				to be scanned
 *		chartoken (O)	char * to null-terminated string returning
 *				either the first (non-whitespace) character
 *				of expression if that char isn't \, or else
 *				the \ and everything following it up to
 *				the next non-alphabetic character (but at
 *				least one char following the \ even if
 *				it's non-alpha)
 * --------------------------------------------------------------------------
 * Returns:	( char * )	ptr to the first char of expression
 *				past returned chartoken,
 *				or NULL for any parsing error.
 * --------------------------------------------------------------------------
 * Notes:     o	Does *not* skip leading whitespace, but simply
 *		returns any whitespace character as the next character.
 * ======================================================================= */
/* --- entry point --- */
char	*texchar ( char *expression, char *chartoken )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
int	esclen = 0,				/*length of escape sequence*/
	maxesclen = 128;			/* max len of esc sequence */
char	*ptoken = chartoken;			/* ptr into chartoken */
int	iprefix = 0;				/* prefix index */
static	char *prefixes[] =			/*e.g., \big followed by ( */
	{ /* "\\left", "\\right", */
	  "\\big",  "\\Big",  "\\bigg",  "\\Bigg",
	  "\\bigl", "\\Bigl", "\\biggl", "\\Biggl",
	  "\\bigr", "\\Bigr", "\\biggr", "\\Biggr", NULL };
static	char *starred[] =			/* may be followed by * */
	{ "\\hspace",  "\\!",  NULL };
/* -------------------------------------------------------------------------
just return the next char if it's not \
-------------------------------------------------------------------------- */
/* --- error check for end-of-string --- */
*ptoken = '\000';				/* init in case of error */
if ( expression == NULL ) return(NULL);		/* nothing to scan */
if ( *expression == '\000' ) return(NULL);	/* nothing to scan */
/* --- always returning first character (either \ or some other char) --- */
*ptoken++ = *expression++;			/* here's first character */
/* --- if first char isn't \, then just return it to caller --- */
if ( !isthischar(*(expression-1),ESCAPE) )	/* not a \, so return char */
  { *ptoken = '\000';				/* add a null terminator */
    goto end_of_job; }				/* ptr past returned char */
if ( *expression == '\000' )			/* \ is very last char */
  { *chartoken = '\000';			/* flush bad trailing \ */
    return(NULL); }				/* and signal end-of-job */
/* -------------------------------------------------------------------------
we have an escape sequence, so return all alpha chars following \
-------------------------------------------------------------------------- */
/* --- accumulate chars until first non-alpha char found --- */
for ( ; isalpha(*expression); esclen++ )	/* till first non-alpha... */
  { if ( esclen < maxesclen-3 )			/* more room in chartoken */
      *ptoken++ = *expression;			/*copy alpha char, bump ptr*/
    expression++; }				/* bump expression ptr */
/* --- if we have a prefix, append next texchar, e.g., \big( --- */
*ptoken = '\000';				/* set null for compare */
for ( iprefix=0; prefixes[iprefix] != NULL; iprefix++ ) /* run thru list */
 if ( strcmp(chartoken,prefixes[iprefix]) == 0 ) /* have an exact match */
  { char nextchar[256];  int nextlen=0;		/* texchar after prefix */
    skipwhite(expression);			/* skip space after prefix*/
    expression = texchar(expression,nextchar);	/* get nextchar */
    if ( (nextlen = strlen(nextchar)) > 0 )	/* #chars in nextchar */
      { strcpy(ptoken,nextchar);		/* append nextchar */
        ptoken += strlen(nextchar);		/* point to null terminator*/
        esclen += strlen(nextchar); }		/* and bump escape length */
    break; }					/* stop checking prefixes */
/* --- every \ must be followed by at least one char, e.g., \[ --- */
if ( esclen < 1 )				/* \ followed by non-alpha */
  *ptoken++ = *expression++;			/*copy non-alpha, bump ptrs*/
*ptoken = '\000';				/* null-terminate token */
/* --- check for \hspace* or other starred commands --- */
for ( iprefix=0; starred[iprefix] != NULL; iprefix++ ) /* run thru list */
 if ( strcmp(chartoken,starred[iprefix]) == 0 )	/* have an exact match */
  if ( *expression == '*' )			/* follows by a * */
   { *ptoken++ = *expression++;			/* copy * and bump ptr */
     *ptoken = '\000';				/* null-terminate token */
     break; }					/* stop checking */
/* --- respect spaces in text mode, except first space after \escape --- */
if ( esclen >= 1 ) {				/*only for alpha \sequences*/
  if ( istextmode )				/* in \rm or \it text mode */
   if ( isthischar(*expression,WHITEDELIM) )	/* delim follows \sequence */
    expression++; }				/* so flush delim */
/* --- back to caller --- */
end_of_job:
  if ( msgfp!=NULL && msglevel>=999 )
    { fprintf(msgfp,"texchar> returning token = \"%s\"\n",chartoken);
      fflush(msgfp); }
  return ( expression );			/*ptr to 1st non-alpha char*/
} /* --- end-of-function texchar() --- */


/* ==========================================================================
 * Function:	texsubexpr (expression,subexpr,maxsubsz,
 *		left,right,isescape,isdelim)
 * Purpose:	scans expression, returning everything between a balanced
 *		left{...right} subexpression if the first non-whitespace
 *		char of expression is an (escaped or unescaped) left{,
 *		or just the next texchar() otherwise,
 *		and a pointer to the first expression char past that.
 * --------------------------------------------------------------------------
 * Arguments:	expression (I)	char * to first char of null-terminated
 *				string containing valid LaTeX expression
 *				to be scanned
 *		subexpr (O)	char * to null-terminated string returning
 *				either everything between a balanced {...}
 *				subexpression if the first char is {,
 *				or the next texchar() otherwise.
 *		maxsubsz (I)	int containing max #bytes returned
 *				in subexpr buffer (0 means unlimited)
 *		left (I)	char * specifying allowable left delimiters
 *				that begin subexpression, e.g., "{[(<"
 *		right (I)	char * specifying matching right delimiters
 *				in the same order as left, e.g., "}])>"
 *		isescape (I)	int controlling whether escaped and/or
 *				unescaped left,right are matched;
 *				see isbrace() comments below for details.
 *		isdelim (I)	int containing true (non-zero) to return
 *				the leading left and trailing right delims
 *				(if any were found) along with subexpr,
 *				or containing false=0 to return subexpr
 *				without its delimiters
 * --------------------------------------------------------------------------
 * Returns:	( char * )	ptr to the first char of expression
 *				past returned subexpr (see Notes),
 *				or NULL for any parsing error.
 * --------------------------------------------------------------------------
 * Notes:     o	If subexpr is of the form left{...right},
 *		the outer {}'s are returned as part of subexpr
 *		if isdelim is true; if isdelim is false the {}'s aren't
 *		returned.  In either case the returned pointer is
 *		*always* bumped past the closing right}, even if
 *		that closing right} isn't returned in subexpr.
 *	      o	If subexpr is not of the form left{...right},
 *		the returned pointer is on the character immediately
 *		following the last character returned in subexpr
 *	      o	\. acts as LaTeX \right. and matches any \left(
 *		And it also acts as a LaTeX \left. and matches any \right)
 * ======================================================================= */
/* --- entry point --- */
char	*texsubexpr ( char *expression, char *subexpr, int maxsubsz,
	char *left, char *right, int isescape, int isdelim )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
char	*texchar();		/*next char (or \sequence) from expression*/
char	*leftptr, leftdelim[256] = "(\000", /* left( found in expression */
	rightdelim[256] = ")\000"; /* and matching right) */
char	*origexpression=expression, *origsubexpr=subexpr; /*original inputs*/
char	*strtexchr(), *texleft(); /* check for \left, and get it */
int	gotescape = 0,		/* true if leading char of expression is \ */
	prevescape = 0;		/* while parsing, true if preceding char \ */
int	isbrace();		/* check for left,right braces */
int	isanyright = 1;		/* true matches any right with left, (...] */
int	isleftdot = 0;		/* true if left brace is a \. */
int	nestlevel = 1;		/* current # of nested braces */
int	subsz=0 /*,maxsubsz=MAXSUBXSZ*/; /*#chars in returned subexpr buffer*/
/* -------------------------------------------------------------------------
skip leading whitespace and just return the next char if it's not {
-------------------------------------------------------------------------- */
/* --- skip leading whitespace and error check for end-of-string --- */
*subexpr = '\000';				/* init in case of error */
if ( expression == NULL ) return(NULL);		/*can't dereference null ptr*/
skipwhite(expression);				/* leading whitespace gone */
if ( *expression == '\000' ) return(NULL);	/* nothing left to scan */
/* --- set maxsubsz --- */
if ( maxsubsz < 1 ) maxsubsz = MAXSUBXSZ-2;	/* input 0 means unlimited */
/* --- check for escape --- */
if ( isthischar(*expression,ESCAPE) )		/* expression is escaped */
  gotescape = 1;				/* so set flag accordingly */
/* --- check for \left...\right --- */
if ( gotescape )				/* begins with \ */
 if ( memcmp(expression+1,"left",4) )		/* and followed by left */
  if ( strchr(left,'l') != NULL )		/* caller wants \left's */
   if ( strtexchr(expression,"\\left") == expression ) /*expression=\left...*/
    { char *pright = texleft(expression,subexpr,maxsubsz, /* find ...\right*/
	(isdelim?NULL:leftdelim),rightdelim);
      if ( isdelim ) strcat(subexpr,rightdelim); /* caller wants delims */
      return ( pright );			/*back to caller past \right*/
    } /* --- end-of-if(expression=="\\left") --- */
/* --- if first char isn't left{ or script, just return it to caller --- */
if ( !isbrace(expression,left,isescape) ) {	/* not a left{ */
  if ( !isthischar(*expression,SCRIPTS) )	/* and not a script */
    return ( texchar(expression,subexpr) );	/* next char to caller */
  else /* --- kludge for super/subscripts to accommodate texscripts() --- */
    { *subexpr++ = *expression;			/* signal script */
      *subexpr = '\000';			/* null-terminate subexpr */
      return ( expression ); } }		/* leave script in stream */
/* --- extract left and find matching right delimiter --- */
*leftdelim  = *(expression+gotescape);		/* the left( in expression */
if ( (gotescape && *leftdelim == '.')		/* we have a left \. */
||   (gotescape && isanyright) )		/*or are matching any right*/
  { isleftdot = 1;				/* so just set flag */
    *leftdelim = '\000'; }			/* and reset leftdelim */
else						/* find matching \right */
  if ( (leftptr=strchr(left,*leftdelim)) != NULL ) /* ptr to that left( */
    *rightdelim = right[(int)(leftptr-left)];	/* get the matching right) */
  else						/* can't happen -- pgm bug */
    return ( NULL );				/*just signal eoj to caller*/
/* -------------------------------------------------------------------------
accumulate chars between balanced {}'s, i.e., till nestlevel returns to 0
-------------------------------------------------------------------------- */
/* --- first initialize by bumping past left{ or \{ --- */
if ( isdelim )   *subexpr++ = *expression++;	/*caller wants { in subexpr*/
  else expression++;				/* always bump past left{ */
if ( gotescape ) {				/*need to bump another char*/
  if ( isdelim ) *subexpr++ = *expression++;	/* caller wants char, too */
  else expression++; }				/* else just bump past it */
/* --- set maximum size for numerical arguments --- */
if ( 0 )					/* check turned on or off? */
 if ( !isescape && !isdelim )			/*looking for numerical arg*/
  maxsubsz = 96;				/* set max arg size */
/* --- search for matching right} --- */
while ( 1 )					/*until balanced right} */
  {
  /* --- error check for end-of-string --- */
  if ( *expression == '\000' )			/* premature end-of-string */
    { if ( 0 && (!isescape && !isdelim) )	/*looking for numerical arg,*/
	{ expression = origexpression;		/* so end-of-string is error*/
	  subexpr = origsubexpr; }		/* so reset all ptrs */
      if ( isdelim ) {				/* generate fake right */
	if ( gotescape )			/* need escaped right */
	  { *subexpr++ = '\\';			/* set escape char */
	    *subexpr++ = '.'; }			/* and fake \right. */
	else					/* escape not wanted */
	    *subexpr++ = *rightdelim; }		/* so fake actual right */
      *subexpr = '\000';			/* null-terminate subexpr */
      return ( expression ); }			/* back with final token */
  /* --- check preceding char for escape --- */
  if ( isthischar(*(expression-1),ESCAPE) )	/* previous char was \ */
	prevescape = 1-prevescape;		/* so flip escape flag */
  else	prevescape = 0;				/* or turn flag off */
  /* --- check for { and } (un/escaped as per leading left) --- */
  if ( gotescape == prevescape )		/* escaped iff leading is */
    { /* --- check for (closing) right delim and see if we're done --- */
      if ( isthischar(*expression,rightdelim)	/* found a right} */
      ||   (isleftdot && isthischar(*expression,right)) /*\left. matches all*/
      ||   (prevescape && isthischar(*expression,".")) ) /*or found \right. */
        if ( --nestlevel < 1 )			/*\right balances 1st \left*/
	  { if ( isdelim ) 			/*caller wants } in subexpr*/
	      *subexpr++ = *expression;		/* so end subexpr with } */
	    else				/*check for \ before right}*/
	      if ( prevescape )			/* have unwanted \ */
		*(subexpr-1) = '\000';		/* so replace it with null */
	    *subexpr = '\000';			/* null-terminate subexpr */
	    return ( expression+1 ); }		/* back with char after } */
      /* --- check for (another) left{ --- */
      if ( isthischar(*expression,leftdelim)	/* found another left{ */
      ||   (isleftdot && isthischar(*expression,left)) ) /* any left{ */
	nestlevel++;
    } /* --- end-of-if(gotescape==prevescape) --- */
  /* --- not done, so copy char to subexpr and continue with next char --- */
  if ( ++subsz < maxsubsz-5 )			/* more room in subexpr */
    *subexpr++ = *expression;			/* so copy char and bump ptr*/
  expression++;					/* bump expression ptr */
  } /* --- end-of-while(1) --- */
} /* --- end-of-function texsubexpr() --- */


/* ==========================================================================
 * Function:	texleft (expression,subexpr,maxsubsz,ldelim,rdelim)
 * Purpose:	scans expression, starting after opening \left,
 *		and returning ptr after matching closing \right.
 *		Everything between is returned in subexpr, if given.
 *		Likewise, if given, ldelim returns delimiter after \left
 *		and rdelim returns delimiter after \right.
 *		If ldelim is given, the returned subexpr doesn't include it.
 *		If rdelim is given, the returned pointer is after that delim.
 * --------------------------------------------------------------------------
 * Arguments:	expression (I)	char * to first char of null-terminated
 *				string immediately following opening \left
 *		subexpr (O)	char * to null-terminated string returning
 *				either everything between balanced
 *				\left ... \right.  If leftdelim given,
 *				subexpr does _not_ contain that delimiter.
 *		maxsubsz (I)	int containing max #bytes returned
 *				in subexpr buffer (0 means unlimited)
 *		ldelim (O)	char * returning delimiter following
 *				opening \left
 *		rdelim (O)	char * returning delimiter following
 *				closing \right
 * --------------------------------------------------------------------------
 * Returns:	( char * )	ptr to the first char of expression
 *				past closing \right, or past closing
 *				right delimiter if rdelim!=NULL,
 *				or NULL for any error.
 * --------------------------------------------------------------------------
 * Notes:     o
 * ======================================================================= */
/* --- entry point --- */
char	*texleft ( char *expression, char *subexpr, int maxsubsz,
	char *ldelim, char *rdelim )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
char	*texchar(),			/* get delims after \left,\right */
	*strtexchr(), *pright=expression; /* locate matching \right */
static	char left[16]="\\left", right[16]="\\right"; /* tex delimiters */
int	sublen = 0;			/* #chars between \left...\right */
/* -------------------------------------------------------------------------
initialization
-------------------------------------------------------------------------- */
/* --- init output --- */
if ( subexpr != NULL ) *subexpr = '\000'; /* init subexpr, if given */
if ( ldelim  != NULL ) *ldelim  = '\000'; /* init ldelim,  if given */
if ( rdelim  != NULL ) *rdelim  = '\000'; /* init rdelim,  if given */
/* --- check args --- */
if ( expression == NULL ) goto end_of_job; /* no input supplied */
if ( *expression == '\000' ) goto end_of_job; /* nothing after \left */
/* --- determine left delimiter  --- */
if ( ldelim != NULL )			/* caller wants left delim */
 { skipwhite(expression);		/* interpret \left ( as \left( */
   expression = texchar(expression,ldelim); } /*delim from expression*/
/* -------------------------------------------------------------------------
locate \right balancing opening \left
-------------------------------------------------------------------------- */
/* --- first \right following \left --- */
if ( (pright=strtexchr(expression,right)) /* look for \right after \left */
!=   NULL ) {				/* found it */
 /* --- find matching \right by pushing past any nested \left's --- */
 char *pleft = expression;		/* start after first \left( */
 while ( 1 ) {				/*break when matching \right found*/
  /* -- locate next nested \left if there is one --- */
  if ( (pleft=strtexchr(pleft,left))	/* find next \left */
  ==   NULL ) break;			/*no more, so matching \right found*/
  pleft += strlen(left);		/* push ptr past \left token */
  if ( pleft >= pright ) break;		/* not nested if \left after \right*/
  /* --- have nested \left, so push forward to next \right --- */
  if ( (pright=strtexchr(pright+strlen(right),right)) /* find next \right */
  ==   NULL ) break;			/* ran out of \right's */
  } /* --- end-of-while(1) --- */
 } /* --- end-of-if(pright!=NULL) --- */
/* --- set subexpression length, push pright past \right --- */
if ( pright != (char *)NULL )		/* found matching \right */
 { sublen = (int)(pright-expression);	/* #chars between \left...\right */
   pright += strlen(right); }		/* so push pright past \right */
/* -------------------------------------------------------------------------
get rightdelim and subexpr between \left...\right
-------------------------------------------------------------------------- */
/* --- get delimiter following \right --- */
if ( rdelim != NULL ) {			/* caller wants right delim */
 if ( pright == (char *)NULL )		/* assume \right. at end of exprssn*/
  { strcpy(rdelim,".");			/* set default \right. */
    sublen = strlen(expression);	/* use entire remaining expression */
    pright = expression + sublen; }	/* and push pright to end-of-string*/
 else					/* have explicit matching \right */
  { skipwhite(pright);			/* interpret \right ) as \right) */
    pright = texchar(pright,rdelim);	/* pull delim from expression */
    if ( *rdelim == '\000' ) strcpy(rdelim,"."); } } /* or set \right. */
/* --- get subexpression between \left...\right --- */
if ( sublen > 0 )			/* have subexpr */
 if ( subexpr != NULL ) {		/* and caller wants it */
  if ( maxsubsz > 0 ) sublen = min2(sublen,maxsubsz-1); /* max buffer size */
  memcpy(subexpr,expression,sublen);	/* stuff between \left...\right */
  subexpr[sublen] = '\000'; }		/* null-terminate subexpr */
end_of_job:
  if ( msglevel>=99 && msgfp!=NULL )
    { fprintf(msgfp,"texleft> ldelim=%s, rdelim=%s, subexpr=%.128s\n",
      (ldelim==NULL?"none":ldelim),(rdelim==NULL?"none":rdelim),
      (subexpr==NULL?"none":subexpr)); fflush(msgfp); }
  return ( pright );
} /* --- end-of-function texleft --- */


/* ==========================================================================
 * Function:	texscripts ( expression, subscript, superscript, which )
 * Purpose:	scans expression, returning subscript and/or superscript
 *		if expression is of the form _x^y or ^{x}_{y},
 *		or any (valid LaTeX) permutation of the above,
 *		and a pointer to the first expression char past "scripts"
 * --------------------------------------------------------------------------
 * Arguments:	expression (I)	char * to first char of null-terminated
 *				string containing valid LaTeX expression
 *				to be scanned
 *		subscript (O)	char * to null-terminated string returning
 *				subscript (without _), if found, or "\000"
 *		superscript (O)	char * to null-terminated string returning
 *				superscript (without ^), if found, or "\000"
 *		which (I)	int containing 1 for subscript only,
 *				2 for superscript only, >=3 for either/both
 * --------------------------------------------------------------------------
 * Returns:	( char * )	ptr to the first char of expression
 *				past returned "scripts" (unchanged
 *				except for skipped whitespace if
 *				neither subscript nor superscript found),
 *				or NULL for any parsing error.
 * --------------------------------------------------------------------------
 * Notes:     o	an input expression like ^a^b_c will return superscript="b",
 *		i.e., totally ignoring all but the last "script" encountered
 * ======================================================================= */
/* --- entry point --- */
char	*texscripts ( char *expression, char *subscript,
			char *superscript, int which )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
char	*texsubexpr();		/* next subexpression from expression */
int	gotsub=0, gotsup=0;	/* check that we don't eat, e.g., x_1_2 */
/* -------------------------------------------------------------------------
init "scripts"
-------------------------------------------------------------------------- */
if ( subscript != NULL ) *subscript = '\000';	/*init in case no subscript*/
if ( superscript!=NULL ) *superscript = '\000';	/*init in case no super*/
/* -------------------------------------------------------------------------
get subscript and/or superscript from expression
-------------------------------------------------------------------------- */
while ( expression != NULL ) {
  skipwhite(expression);			/* leading whitespace gone */
  if ( *expression == '\000' ) return(expression); /* nothing left to scan */
  if ( isthischar(*expression,SUBSCRIPT)	/* found _ */
  &&   (which==1 || which>2 ) )			/* and caller wants it */
    { if ( gotsub				/* found 2nd subscript */
      ||   subscript == NULL ) break;		/* or no subscript buffer */
      gotsub = 1;				/* set subscript flag */
      expression = texsubexpr(expression+1,subscript,0,"{","}",0,0); }
  else						/* no _, check for ^ */
    if ( isthischar(*expression,SUPERSCRIPT)	/* found ^ */
    &&   which>=2  )				/* and caller wants it */
      {	if ( gotsup				/* found 2nd superscript */
	||   superscript == NULL ) break;	/* or no superscript buffer*/
	gotsup = 1;				/* set superscript flag */
	expression = texsubexpr(expression+1,superscript,0,"{","}",0,0); }
    else					/* neither _ nor ^ */
      return ( expression );			/*return ptr past "scripts"*/
  } /* --- end-of-while(expression!=NULL) --- */
return ( expression );
} /* --- end-of-function texscripts() --- */


/* ==========================================================================
 * Function:	isbrace ( expression, braces, isescape )
 * Purpose:	checks leading char(s) of expression for a brace,
 *		either escaped or unescaped depending on isescape,
 *		except that { and } are always matched, if they're
 *		in braces, regardless of isescape.
 * --------------------------------------------------------------------------
 * Arguments:	expression (I)	char * to first char of null-terminated
 *				string containing a valid LaTeX expression
 *				whose leading char(s) are checked for braces
 *				that begin subexpression, e.g., "{[(<"
 *		braces (I)	char * specifying matching brace delimiters
 *				to be checked for, e.g., "{[(<" or "}])>"
 *		isescape (I)	int containing 0 to match only unescaped
 *				braces, e.g., (...) or {...}, etc,
 *				or containing 1 to match only escaped
 *				braces, e.g., \(...\) or \[...\], etc,
 *				or containing 2 to match either.
 *				But note: if {,} are in braces
 *				then they're *always* matched whether
 *				escaped or not, regardless of isescape.
 * --------------------------------------------------------------------------
 * Returns:	( int )		1 if the leading char(s) of expression
 *				is a brace, or 0 if not.
 * --------------------------------------------------------------------------
 * Notes:     o
 * ======================================================================= */
/* --- entry point --- */
int	isbrace ( char *expression, char *braces, int isescape )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
int	gotescape = 0,		/* true if leading char is an escape */
	gotbrace = 0;		/*true if first non-escape char is a brace*/
/* -------------------------------------------------------------------------
check for brace
-------------------------------------------------------------------------- */
/* --- first check for end-of-string or \= ligature --- */
if ( *expression == '\000'			/* nothing to check */
||   isligature ) goto end_of_job;		/* have a \= ligature */
/* --- check leading char for escape --- */
if ( isthischar(*expression,ESCAPE) )		/* expression is escaped */
  { gotescape = 1;				/* so set flag accordingly */
    expression++; }				/* and bump past escape */
/* --- check (maybe next char) for brace --- */
if ( isthischar(*expression,braces) )		/* expression is braced */
  gotbrace = 1;					/* so set flag accordingly */
if ( gotescape && *expression == '.' )		/* \. matches any brace */
  gotbrace = 1;					/* set flag */
/* --- check for TeX brace { or } --- */
if ( gotbrace && isthischar(*expression,"{}") )	/*expression has TeX brace*/
  if ( isescape ) isescape = 2;			/* reset escape flag */
/* -------------------------------------------------------------------------
back to caller
-------------------------------------------------------------------------- */
end_of_job:
 if ( msglevel>=999 && msgfp!=NULL )
  { fprintf(msgfp,"isbrace> expression=%.8s, gotbrace=%d (isligature=%d)\n",
    expression,gotbrace,isligature); fflush(msgfp); }
 if ( gotbrace &&				/* found a brace */
     ( isescape==2 ||				/* escape irrelevant */
       gotescape==isescape )			/* un/escaped as requested */
   ) return ( 1 );  return ( 0 );		/* return 1,0 accordingly */
} /* --- end-of-function isbrace() --- */


/* ==========================================================================
 * Function:	preamble ( expression, size, subexpr )
 * Purpose:	parses $-terminated preamble, if present, at beginning
 *		of expression, re-setting size if necessary, and
 *		returning any other parameters besides size in subexpr.
 * --------------------------------------------------------------------------
 * Arguments:	expression (I)	char * to first char of null-terminated
 *				string containing LaTeX expression possibly
 *				preceded by $-terminated preamble
 *		size (I/O)	int *  containing 0-4 default font size,
 *				and returning size modified by first
 *				preamble parameter (or unchanged)
 *		subexpr(O)	char *  returning any remaining preamble
 *				parameters past size
 * --------------------------------------------------------------------------
 * Returns:	( char * )	ptr to first char past preamble in expression
 *				or NULL for any parsing error.
 * --------------------------------------------------------------------------
 * Notes:     o	size can be any number >=0. If preceded by + or -, it's
 *		interpreted as an increment to input size; otherwise
 *		it's interpreted as the size.
 *	      o	if subexpr is passed as NULL ptr, then returned expression
 *		ptr will have "flushed" and preamble parameters after size
 * ======================================================================= */
/* --- entry point --- */
char	*preamble ( char *expression, int *size, char *subexpr )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
char	pretext[512], *prep=expression,	/*pream from expression, ptr into it*/
	*dollar, *comma;		/* preamble delimiters */
int	prelen = 0,			/* preamble length */
	sizevalue = 0,			/* value of size parameter */
	isfontsize = 0,			/*true if leading fontsize present*/
	isdelta = 0;			/*true to increment passed size arg*/
/* -------------------------------------------------------------------------
initialization
-------------------------------------------------------------------------- */
if ( subexpr != NULL )			/* caller passed us an address */
  *subexpr = '\000';			/* so init assuming no preamble */
if ( expression == NULL ) goto end_of_job; /* no input */
if ( *expression == '\000' ) goto end_of_job; /* input is an empty string */
/* -------------------------------------------------------------------------
process preamble if present
-------------------------------------------------------------------------- */
/*process_preamble:*/
if ( (dollar=strchr(expression,'$'))	/* $ signals preceding preamble */
!=   NULL ) {				/* found embedded $ */
 if ( (prelen = (int)(dollar-expression)) /*#chars in expression preceding $*/
 > 0 ) {				/* must have preamble preceding $ */
  if ( prelen < 65 ) {			/* too long for a prefix */
   memcpy(pretext,expression,prelen);	/* local copy of preamble */
   pretext[prelen] = '\000';		/* null-terminated */
   if ( strchr(pretext,*(ESCAPE))==NULL	/*shouldn't be an escape in preamble*/
   &&   strchr(pretext,'{') == NULL ) {	/*shouldn't be a left{ in preamble*/
    /* --- skip any leading whitespace  --- */
    prep = pretext;			/* start at beginning of preamble */
    skipwhite(prep);			/* skip any leading white space */
    /* --- check for embedded , or leading +/- (either signalling size) --- */
    if ( isthischar(*prep,"+-") )	/* have leading + or - */
     isdelta = 1;			/* so use size value as increment */
    comma = strchr(pretext,',');	/* , signals leading size param */
    /* --- process leading size parameter if present --- */
    if ( comma != NULL			/* size param explicitly signalled */
    ||   isdelta || isdigit(*prep) ) {	/* or inferred implicitly */
      /* --- parse size parameter and reset size accordingly --- */
      if( comma != NULL ) *comma = '\000';/*, becomes null, terminating size*/
      sizevalue = atoi(prep);		/* convert size string to integer */
      if ( size != NULL )		/* caller passed address for size */
	*size = (isdelta? *size+sizevalue : sizevalue); /* so reset size */
      /* --- finally, set flag and shift size parameter out of preamble --- */
      isfontsize = 1;			/*set flag showing font size present*/
      if ( comma != NULL )		/*2/15/12-isn't this superfluous???*/
        {strsqueezep(pretext,comma+1);}	/* squeeze out leading size param */
     } /* --- end-of-if(comma!=NULL||etc) --- */
    /* --- copy any preamble params following size to caller's subexpr --- */
    if ( comma != NULL || !isfontsize )	/*preamb contains params past size*/
     if ( subexpr != NULL )		/* caller passed us an address */
      strcpy(subexpr,pretext);		/*so return extra params to caller*/
    /* --- finally, set prep to shift preamble out of expression --- */
    prep = expression + prelen+1;	/* set prep past $ in expression */
    } /* --- end-of-if(strchr(pretext,*ESCAPE)==NULL) --- */
   } /* --- end-of-if(prelen<65) --- */
  } /* --- end-of-if(prelen>0) --- */
 else {					/* $ is first char of expression */
  int ndollars = 0;			/* number of $...$ pairs removed */
  prep = expression;			/* start at beginning of expression*/
  while ( *prep == '$' ) {		/* remove all matching $...$'s */
   int	explen = strlen(prep)-1;	/* index of last char in expression*/
   if ( explen < 2 ) break;		/* no $...$'s left to remove */
   if ( prep[explen] != '$' ) break;	/* unmatched $ */
   prep[explen] = '\000';		/* remove trailing $ */
   prep++;				/* and remove matching leading $ */
   ndollars++;				/* count another pair removed */
   } /* --- end-of-while(*prep=='$') --- */
  ispreambledollars = ndollars;		/* set flag to fix \displaystyle */
  if ( ndollars == 1 )			/* user submitted $...$ expression */
    isdisplaystyle = 0;			/* so set \textstyle */
  if ( ndollars > 1 )			/* user submitted $$...$$ */
    isdisplaystyle = 2;			/* so set \displaystyle */
  /*goto process_preamble;*/		/*check for preamble after leading $*/
  } /* --- end-of-if/else(prelen>0) --- */
 } /* --- end-of-if(dollar!=NULL) --- */
/* -------------------------------------------------------------------------
back to caller
-------------------------------------------------------------------------- */
end_of_job:
  return ( prep );			/*expression, or ptr past preamble*/
} /* --- end-of-function preamble() --- */


/* ==========================================================================
 * Function:	mimeprep ( expression )
 * Purpose:	preprocessor for mimeTeX input, e.g.,
 *		(a) removes comments,
 *		(b) converts \left( to \( and \right) to \),
 *		(c) xlates &html; special chars to equivalent latex
 *		Should only be called once (after unescape_url())
 * --------------------------------------------------------------------------
 * Arguments:	expression (I/O) char * to first char of null-terminated
 *				string containing mimeTeX/LaTeX expression,
 *				and returning preprocessed string
 * --------------------------------------------------------------------------
 * Returns:	( char * )	ptr to input expression,
 *				or NULL for any parsing error.
 * --------------------------------------------------------------------------
 * Notes:     o
 * ======================================================================= */
/* --- entry point --- */
char	*mimeprep ( char *expression )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
char	*expptr=expression,		/* ptr within expression */
	*tokptr=NULL,			/*ptr to token found in expression*/
	*texsubexpr(), argval[8192];	/*parse for macro args after token*/
char	*strchange();			/* change leading chars of string */
int	strreplace();			/* replace nnn with actual num, etc*/
char	*strwstr();			/*use strwstr() instead of strstr()*/
char	*findbraces();			/*find left { and right } for \atop*/
int	idelim=0,			/* left- or right-index */
	isymbol=0;			/*symbols[],rightcomment[],etc index*/
int	xlateleft = 0;			/* true to xlate \left and \right */
/* ---
 * comments
 * -------- */
char	*leftptr=NULL;			/* find leftcomment in expression */
static	char *leftcomment = "%%",	/* open comment */
	*rightcomment[] = {"\n", "%%", NULL}; /* close comments */
/* ---
 * special long (more than 1-char) \left and \right delimiters
 * ----------------------------------------------------------- */
static	char *leftfrom[] =		/* xlate any \left suffix... */
   { "\\|",				/* \left\| */
     "\\{",				/* \left\{ */
     "\\langle",			/* \left\langle */
     NULL } ; /* --- end-of-leftfrom[] --- */
static	char *leftto[] =		/* ...to this instead */
   { "=",				/* = */
     "{",				/* { */
     "<",				/* < */
     NULL } ; /* --- end-of-leftto[] --- */
static	char *rightfrom[] =		/* xlate any \right suffix... */
   { "\\|",				/* \right\| */
     "\\}",				/* \right\} */
     "\\rangle",			/* \right\rangle */
     NULL } ; /* --- end-of-rightfrom[] --- */
static	char *rightto[] =		/* ...to this instead */
   { "=",				/* = */
     "}",				/* } */
     ">",				/* > */
     NULL } ; /* --- end-of-rightto[] --- */
/* ---
 * { \atop }-like commands
 * ----------------------- */
char	*atopsym=NULL;			/* atopcommands[isymbol] */
static	char *atopcommands[] =		/* list of {a+b\command c+d}'s */
   { "\\over",				/* plain tex for \frac */
     "\\choose",			/* binomial coefficient */
   #ifndef NOATOP			/*noatop preserves old mimeTeX rule*/
     "\\atop",
   #endif
     NULL } ; /* --- end-of-atopcommands[] --- */
static	char *atopdelims[] =		/* delims for atopcommands[] */
   { NULL, NULL,			/* \\over has no delims */
     "\\left(", "\\right)",		/* \\choose has ( ) delims*/
   #ifndef NOATOP			/*noatop preserves old mimeTeX rule*/
     NULL, NULL,			/* \\atop has no delims */
   #endif
     NULL, NULL } ; /* --- end-of-atopdelims[] --- */
/* ---
 * html special/escape chars converted to latex equivalents
 * -------------------------------------------------------- */
char	*htmlsym=NULL;			/* symbols[isymbol].html */
static	struct { char *html; char *args; char *latex; } symbols[] =
 { /* --------------------------------------------
     user-supplied newcommands
   -------------------------------------------- */
   #ifdef NEWCOMMANDS			/* -DNEWCOMMANDS=\"filename.h\" */
     #include NEWCOMMANDS
   #endif
   /* --------------------------------------------
     Specials        termchar  value...
   -------------------------------------------- */
   { "\\version",	NULL, "{\\small\\red\\text \\fbox{\\begin{gather}"
	"mime\\TeX version \\versionnumber \\\\"
	"last revised \\revisiondate \\\\ \\copyrighttext \\\\"
	"see \\homepagetext for details \\end{gather}}}" },
   { "\\copyright",	NULL,
	"{\\small\\red\\text \\fbox{\\begin{gather}"
	"mimeTeX \\copyrighttext \\\\"
	"see \\homepagetext for details \\end{gather}}}" },
   { "\\versionnumber",	NULL, "{\\text " VERSION "}" },
   { "\\revisiondate",	NULL, "{\\text " REVISIONDATE "}" },
   { "\\copyrighttext",	NULL, "{\\text " COPYRIGHTTEXT ".}" },
   { "\\homepagetext",	NULL,
	"{\\text http://www.forkosh.com/mimetex.html}" },
   /* --------------------------------------------
     Cyrillic  termchar  mimeTeX equivalent...
   -------------------------------------------- */
   { "\\\'G",	"embed\\","{\\acute{G}}" },
   { "\\\'g",	"embed\\","{\\acute{g}}" },
   { "\\\'K",	"embed\\","{\\acute{K}}" },
   { "\\\'k",	"embed\\","{\\acute{k}}" },
   { "\\u U",	"embed\\","{\\breve{U}}" },
   { "\\u u",	"embed\\","{\\breve{u}}" },
   /*{ "\\\"E",	"embed\\","{\\ddot{E}}" },*/
   /*{ "\\\"e",	"embed\\","{\\ddot{e}}" },*/
   { "\\\"I",	"embed\\","{\\ddot{\\=I}}" },
   { "\\\"\\i",	"embed\\","{\\ddot{\\=\\i}}" },
   /* --------------------------------------------
     LaTeX Macro #args,default  template...
   -------------------------------------------- */
   { "\\lvec",	"2n",	"{#2_1,\\cdots,#2_{#1}}" },
   { "\\grave", "1",	"{\\stackrel{\\Huge\\gravesym}{#1}}" }, /* \grave */
   { "\\acute", "1",	"{\\stackrel{\\Huge\\acutesym}{#1}}" }, /* \acute */
   { "\\check", "1",	"{\\stackrel{\\Huge\\checksym}{#1}}" }, /* \check */
   { "\\breve", "1",	"{\\stackrel{\\Huge\\brevesym}{#1}}" }, /* \breve */
   { "\\buildrel","3",	"{\\stackrel{#1}{#3}}" }, /* ignore #2 = \over */
   { "\\overset", NULL,	"\\stackrel" },		/* just an alias */
   { "\\underset", "2",	"\\relstack{#2}{#1}" },	/* reverse args */
   { "\\dfrac", "2",	"{\\frac{#1}{#2}}" },
   { "\\binom", "2",	"{\\begin{pmatrix}{#1}\\\\{#2}\\end{pmatrix}}" },
   { "\\aangle","26",	"{\\boxaccent{#1}{#2}}" },
   { "\\actuarial","2 ","{#1\\boxaccent{6}{#2}}" }, /*comprehensive sym list*/
   { "\\boxaccent","2", "{\\fbox[,#1]{#2}}" },
   { "\\password", "1",	"\000" },
   { "\\comment", "1",	"\000" },
   /* --------------------------------------------
     html char termchar  LaTeX equivalent...
   -------------------------------------------- */
   { "&quot",	";",	"\"" },		/* &quot; is first, &#034; */
   { "&amp",	";",	"&" },
   { "&lt",	";",	"<" },
   { "&gt",	";",	">" },
   /*{ "&#092",	";",	"\\" },*/	/* backslash */
   { "&backslash",";",	"\\" },
   { "&nbsp",	";",	"~" },
   { "&iexcl",	";",	"{\\raisebox{-2}{\\rotatebox{180}{!}}}" },
   { "&brvbar",	";",	"|" },
   { "&plusmn",	";",	"\\pm" },
   { "&sup2",	";",	"{{}^2}" },
   { "&sup3",	";",	"{{}^3}" },
   { "&micro",	";",	"\\mu" },
   { "&sup1",	";",	"{{}^1}" },
   { "&frac14",	";",	"{\\frac14}" },
   { "&frac12",	";",	"{\\frac12}" },
   { "&frac34",	";",	"{\\frac34}" },
   { "&iquest",	";",	"{\\raisebox{-2}{\\rotatebox{180}{?}}}" },
   { "&Acirc",	";",	"{\\rm~\\hat~A}" },
   { "&Atilde",	";",	"{\\rm~\\tilde~A}" },
   { "&Auml",	";",	"{\\rm~\\ddot~A}" },
   { "&Aring",	";",	"{\\rm~A\\limits^{-1$o}}" },
   { "&atilde",	";",	"{\\rm~\\tilde~a}" },
   { "&yuml",	";",	"{\\rm~\\ddot~y}" }, /* &yuml; is last, &#255; */
   { "&#",	";",	"{[\\&\\#nnn?]}" },  /* all other explicit &#nnn's */
   /* --------------------------------------------
     html tag     termchar    LaTeX equivalent...
   -------------------------------------------- */
   { "< br >",    "embed\\i", "\\\\" },
   { "< br / >",  "embed\\i", "\\\\" },
   { "< dd >",    "embed\\i", " \000" },
   { "< / dd >",  "embed\\i", " \000" },
   { "< dl >",    "embed\\i", " \000" },
   { "< / dl >",  "embed\\i", " \000" },
   { "< p >",     "embed\\i", " \000" },
   { "< / p >",   "embed\\i", " \000" },
   /* --------------------------------------------
     garbage      termchar  LaTeX equivalent...
   -------------------------------------------- */
   { "< tex >",   "embed\\i", " \000" },
   { "< / tex >", "embed\\i", " \000" },
   /* --------------------------------------------
     LaTeX   termchar   mimeTeX equivalent...
   -------------------------------------------- */
   { "\\AA",	NULL,	"{\\rm~A\\limits^{-1$o}}" },
   { "\\aa",	NULL,	"{\\rm~a\\limits^{-1$o}}" },
   { "\\bmod",	NULL,	"{\\hspace2{\\rm~mod}\\hspace2}" },
   { "\\mod",	NULL,	"{\\hspace2{\\rm~mod}\\hspace2}" },
   { "\\vdots",	NULL,	"{\\raisebox3{\\rotatebox{90}{\\ldots}}}" },
   { "\\dots",	NULL,	"{\\cdots}" },
   { "\\cdots",	NULL,	"{\\raisebox3{\\ldots}}" },
   { "\\ldots",	NULL,	"{\\fs4.\\hspace1.\\hspace1.}" },
   { "\\ddots",	NULL,	"{\\fs4\\raisebox8.\\hspace1\\raisebox4."
			"\\hspace1\\raisebox0.}"},
   { "\\cong",	NULL,	"{\\raisebox{-4}{\\approx\\atop-}}" },
   { "\\notin",	NULL,	"{\\not\\in}" },
   { "\\neq",	NULL,	"{\\not=}" },
   { "\\ne",	NULL,	"{\\not=}" },
   { "\\pm",	NULL,	"{\\math\\plusminus}" }, /* don't have \bf\pm */
   { "\\mapsto", NULL,	"{\\rule[fs/2]{1}{5+fs}\\hspace{-99}\\to}" },
   { "\\hbar",	NULL,	"{\\compose~h{{\\fs{-1}-\\atop\\vspace3}}}" },
   { "\\angle",	NULL, "{\\compose{\\hspace{3}\\lt}{\\circle(10,15;-80,80)}}"},
   { "\\textcelsius", NULL, "{\\textdegree C}"},
   { "\\textdegree", NULL, "{\\Large^{^{\\tiny\\mathbf o}}}"},
   { "\\cr",	NULL,	"\\\\" },
   /*{ "\\colon",	NULL,	"{:}" },*/
   { "\\iiint",	NULL,	"{\\int\\int\\int}\\limits" },
   { "\\iint",	NULL,	"{\\int\\int}\\limits" },
   { "\\Bigiint", NULL,	"{\\Bigint\\Bigint}\\limits" },
   { "\\bigsqcap",NULL,	"{\\fs{+4}\\sqcap}" },
   { "\\_",	"embed","{\\underline{\\ }}" }, /* displayed underscore */
   { "!`",	NULL,	"{\\raisebox{-2}{\\rotatebox{180}{!}}}" },
   { "?`",	NULL,	"{\\raisebox{-2}{\\rotatebox{180}{?}}}" },
   { "^\'",	"embed","\'" }, /* avoid ^^ when re-xlating \' below */
   { "\'\'\'\'","embed","^{\\fs{-1}\\prime\\prime\\prime\\prime}" },
   { "\'\'\'",	"embed","^{\\fs{-1}\\prime\\prime\\prime}" },
   { "\'\'",	"embed","^{\\fs{-1}\\prime\\prime}" },
   { "\'",	"embed","^{\\fs{-1}\\prime}" },
   { "\\rightleftharpoons",NULL,"{\\rightharpoonup\\atop\\leftharpoondown}" },
   { "\\therefore",NULL,"{\\Huge\\raisebox{-4}{.\\atop.\\,.}}" },
   { "\\LaTeX",	NULL,	"{\\rm~L\\raisebox{3}{\\fs{-1}A}\\TeX}" },
   { "\\TeX",	NULL,	"{\\rm~T\\raisebox{-3}{E}X}" },
   { "\\cyan",	NULL,	"{\\reverse\\red\\reversebg}" },
   { "\\magenta",NULL,	"{\\reverse\\green\\reversebg}" },
   { "\\yellow",NULL,	"{\\reverse\\blue\\reversebg}" },
   { "\\cancel",NULL,	"\\Not" },
   { "\\hhline",NULL,	"\\Hline" },
   { "\\Hline", NULL,	"\\hline\\,\\\\\\hline" },
   /* -----------------------------------------------------------------------
     As per emails with Zbigniew Fiedorowicz <fiedorow@math.ohio-state.edu>
     "Algebra Syntax"  termchar   mimeTeX/LaTeX equivalent...
   ----------------------------------------------------------------------- */
   { "sqrt",	"1",	"{\\sqrt{#1}}" },
   { "sin",	"1",	"{\\sin{#1}}" },
   { "cos",	"1",	"{\\cos{#1}}" },
   { "asin",	"1",	"{\\sin^{-1}{#1}}" },
   { "acos",	"1",	"{\\cos^{-1}{#1}}" },
   { "exp",	"1",	"{{\\rm~e}^{#1}}" },
   { "det",	"1",	"{\\left|{#1}\\right|}" },
   /* --------------------------------------------
     LaTeX Constant    termchar   value...
   -------------------------------------------- */
   { "\\thinspace",	NULL,	"\\," },
   { "\\thinmathspace",	NULL,	"\\," },
   { "\\textwidth",	NULL,	"400" },
   /* --- end-of-table indicator --- */
   { NULL,	NULL,	NULL }
 } ; /* --- end-of-symbols[] --- */
/* ---
 * html &#nn chars converted to latex equivalents
 * ---------------------------------------------- */
int	htmlnum=0;			/* numbers[inum].html */
static	struct { int html; char *latex; } numbers[] =
 { /* ---------------------------------------
    html num  LaTeX equivalent...
   --------------------------------------- */
   { 9,		" " },			/* horizontal tab */
   { 10,	" " },			/* line feed */
   { 13,	" " },			/* carriage return */
   { 32,	" " },			/* space */
   { 33,	"!" },			/* exclamation point */
   { 34,	"\"" },			/* &quot; */
   { 35,	"#" },			/* hash mark */
   { 36,	"$" },			/* dollar */
   { 37,	"%" },			/* percent */
   { 38,	"&" },			/* &amp; */
   { 39,	"\'" },			/* apostrophe (single quote) */
   { 40,	")" },			/* left parenthesis */
   { 41,	")" },			/* right parenthesis */
   { 42,	"*" },			/* asterisk */
   { 43,	"+" },			/* plus */
   { 44,	"," },			/* comma */
   { 45,	"-" },			/* hyphen (minus) */
   { 46,	"." },			/* period */
   { 47,	"/" },			/* slash */
   { 58,	":" },			/* colon */
   { 59,	";" },			/* semicolon */
   { 60,	"<" },			/* &lt; */
   { 61,	"=" },			/* = */
   { 62,	">" },			/* &gt; */
   { 63,	"\?" },			/* question mark */
   { 64,	"@" },			/* commercial at sign */
   { 91,	"[" },			/* left square bracket */
   { 92,	"\\" },			/* backslash */
   { 93,	"]" },			/* right square bracket */
   { 94,	"^" },			/* caret */
   { 95,	"_" },			/* underscore */
   { 96,	"`" },			/* grave accent */
   { 123,	"{" },			/* left curly brace */
   { 124,	"|" },			/* vertical bar */
   { 125,	"}" },			/* right curly brace */
   { 126,	"~" },			/* tilde */
   { 160,	"~" },			/* &nbsp; (use tilde for latex) */
   { 166,	"|" },			/* &brvbar; (broken vertical bar) */
   { 173,	"-" },			/* &shy; (soft hyphen) */
   { 177,	"{\\pm}" },		/* &plusmn; (plus or minus) */
   { 215,	"{\\times}" },		/* &times; (plus or minus) */
   { -999,	NULL }
 } ; /* --- end-of-numbers[] --- */
/* -------------------------------------------------------------------------
first remove comments
-------------------------------------------------------------------------- */
expptr = expression;			/* start search at beginning */
while ( (leftptr=strstr(expptr,leftcomment)) != NULL ) /*found leftcomment*/
  {
  char	*rightsym=NULL;			/* rightcomment[isymbol] */
  expptr = leftptr+strlen(leftcomment);	/* start rightcomment search here */
  /* --- check for any closing rightcomment, in given precedent order --- */
  if ( *expptr != '\000' )		/*have chars after this leftcomment*/
   for(isymbol=0; (rightsym=rightcomment[isymbol]) != NULL; isymbol++)
    if ( (tokptr=strstr(expptr,rightsym)) != NULL ) /*found rightcomment*/
     { tokptr += strlen(rightsym);	/* first char after rightcomment */
       if ( *tokptr == '\000' )		/*nothing after this rightcomment*/
	{ *leftptr = '\000';		/*so terminate expr at leftcomment*/
	  break; }			/* and stop looking for comments */
       *leftptr = '~';			/* replace entire comment by ~ */
       strsqueezep(leftptr+1,tokptr);	/* squeeze out comment */
       goto next_comment; }		/* stop looking for rightcomment */
  /* --- no rightcomment after opening leftcomment --- */
  *leftptr = '\000';			/* so terminate expression */
  /* --- resume search past squeezed-out comment --- */
  next_comment:
    if ( *leftptr == '\000' ) break;	/* reached end of expression */
    expptr = leftptr+1;			/*resume search after this comment*/
  } /* --- end-of-while(leftptr!=NULL) --- */
/* -------------------------------------------------------------------------
run thru table, converting all occurrences of each macro to its expansion
-------------------------------------------------------------------------- */
for(isymbol=0; (htmlsym=symbols[isymbol].html) != NULL; isymbol++)
  {
  int	htmllen = strlen(htmlsym);	/* length of escape, _without_ ; */
  int	isalgebra = isalpha((int)(*htmlsym)); /* leading char alphabetic */
  int	isembedded = 0,			/* true to xlate even if embedded */
	istag=0, isamp=0,		/* true for <tag>, &char; symbols */
	isstrwstr = 0,			/* true to use strwstr() */
	wstrlen = 0;			/* length of strwstr() match */
  char	*aleft="{([<|", *aright="})]>|"; /*left,right delims for alg syntax*/
  char	embedkeywd[99] = "embed",	/* keyword to signal embedded token*/
	embedterm = '\000';		/* char immediately after embed */
  int	embedlen = strlen(embedkeywd);	/* #chars in embedkeywd */
  char	*args = symbols[isymbol].args,	/* number {}-args, optional []-arg */
	*htmlterm = args,		/*if *args nonumeric, then html term*/
	*latexsym = symbols[isymbol].latex, /*latex replacement for htmlsym*/
	errorsym[256];			/*or latexsym may point to error msg*/
  char	abuff[8192];  int iarg,nargs=0;	/* macro expansion params */
  char	wstrwhite[99];			/* whitespace chars for strwstr() */
  skipwhite(htmlsym);			/*skip any bogus leading whitespace*/
  htmllen = strlen(htmlsym);		/* reset length of html token */
  istag = (isthischar(*htmlsym,"<")?1:0); /* html <tag> starts with < */
  isamp = (isthischar(*htmlsym,"&")?1:0); /* html &char; starts with & */
  if ( args != NULL )			/*we have args (or htmlterm) param*/
   if ( *args != '\000' ) {		/* and it's not an empty string */
    if ( strchr("0123456789",*args) != NULL ) /* is 1st char #args=0-9 ? */
     { htmlterm = NULL;			/* if so, then we have no htmlterm */
       *abuff = *args;  abuff[1] = '\000'; /* #args char in ascii buffer */
       nargs = atoi(abuff); }		/* interpret #args to numeric */
    else if ( strncmp(args,embedkeywd,embedlen) == 0 )/*xlate embedded token*/
     { int arglen = strlen(args);	/* length of "embed..." string */
       htmlterm = NULL;			/* if so, then we have no htmlterm */
       isembedded = 1 ;			/* turn on embedded flag */
       if ( arglen > embedlen )		/* have embed "allow escape" flag */
         embedterm = args[embedlen];	/* char immediately after "embed" */
       if (arglen > embedlen+1) {	/* have embed,flag,white for strwstr*/
	 isstrwstr = 1;			/* turn on strwtsr flag */
	 strcpy(wstrwhite,args+embedlen+1); } } /*and set its whitespace arg*/
    } /* --- end-of-if(*args!='\000') --- */
  expptr = expression;			/* re-start search at beginning */
  while ( ( tokptr=(!isstrwstr?strstr(expptr,htmlsym): /* just use strtsr */
  strwstr(expptr,htmlsym,wstrwhite,&wstrlen)) ) /* or use our strwstr */
  != NULL ) {				/* found another sym */
      int  toklen = (!isstrwstr?htmllen:wstrlen); /* length of matched sym */
      char termchar = *(tokptr+toklen),	/* char terminating html sequence */
           prevchar = (tokptr==expptr?' ':*(tokptr-1));/*char preceding html*/
      int  isescaped = (isthischar(prevchar,ESCAPE)?1:0); /* token escaped?*/
      int  escapelen = toklen;		/* total length of escape sequence */
      int  isflush = 0;			/* true to flush (don't xlate) */
      /* --- check odd/even backslashes preceding tokens --- */
      if ( isescaped ) {		/* have one preceding backslash */
	char *p = tokptr-1;		/* ptr to that preceding backslash */
	while ( p != expptr ) {		/* and we may have more preceding */
	  p--; if(!isthischar(*p,ESCAPE))break; /* but we don't, so quit */
	  isescaped = 1-isescaped; } }	/* or flip isescaped flag if we do */
      /* --- init with "trivial" abuff,escapelen from symbols[] table --- */
      *abuff = '\000';			/* default to empty string */
      if ( latexsym != NULL )		/* table has .latex xlation */
       if ( *latexsym != '\000' )	/* and it's not an empty string */
	strcpy(abuff,latexsym);		/* so get local copy */
      if ( !isembedded )		/*embedded sequences not terminated*/
       if ( htmlterm != NULL )		/* sequence may have terminator */
	escapelen += (isthischar(termchar,htmlterm)?1:0); /*add terminator*/
      /* --- don't xlate if we just found prefix of longer symbol, etc --- */
      if ( !isembedded ) {		/* not embedded */
	if ( isescaped )		/* escaped */
	  isflush = 1;			/* set flag to flush escaped token */
	if ( !istag && isalpha((int)termchar) ) /* followed by alpha */
	  isflush = 1;			/* so just a prefix of longer symbol*/
	if ( isalpha((int)(*htmlsym)) )	/* symbol starts with alpha */
          if ( (!isspace(prevchar)&&isalpha(prevchar)) ) /* just a suffix*/
	    isflush = 1; }		/* set flag to flush token */
      if ( isembedded )			/* for embedded token */
       if ( isescaped )			/* and embedded \token escaped */
	if ( !isthischar(embedterm,ESCAPE) ) /* don't xlate escaped \token */
	  isflush = 1;			/* set flag to flush token */
      if ( isflush )			/* don't xlate this token */
	{ expptr = tokptr+1;/*toklen;*/	/* just resume search after token */
	  continue; }			/* but don't replace it */
      /* --- check for &# prefix signalling &#nnn; --- */
      if ( strcmp(htmlsym,"&#") == 0 ) { /* replacing special &#nnn; chars */
       /* --- accumulate chars comprising number following &# --- */
       char anum[32];			/* chars comprising number after &# */
       int  inum = 0;			/* no chars accumulated yet */
       while ( termchar != '\000' ) {	/* don't go past end-of-string */
         if ( !isdigit((int)termchar) ) break; /* and don't go past digits */
         if ( inum > 10 ) break;	/* some syntax error in expression */
         anum[inum] = termchar;		/* accumulate this digit */
         inum++;  toklen++;		/* bump field length, token length */
         termchar = *(tokptr+toklen); }	/* char terminating html sequence */
       anum[inum] = '\000';		/* null-terminate anum */
       escapelen = toklen;		/* length of &#nnn; sequence */
       if ( htmlterm != NULL )		/* sequence may have terminator */
         escapelen += (isthischar(termchar,htmlterm)?1:0); /*add terminator*/
       /* --- look up &#nnn in number[] table --- */
       htmlnum = atoi(anum);		/* convert anum[] to an integer */
       strninit(errorsym,latexsym,128);	/* init error message */
       latexsym = errorsym;		/* init latexsym as error message */
       strreplace(latexsym,"nnn",anum,1); /*place actual &#num in message*/
       for ( inum=0; numbers[inum].html>=0; inum++ ) /* run thru numbers[] */
         if ( htmlnum ==  numbers[inum].html ) { /* till we find a match */
           latexsym = numbers[inum].latex; /* latex replacement */
           break; }			/* no need to look any further */
       if ( latexsym != NULL )		/* table has .latex xlation */
        if ( *latexsym != '\000' )	/* and it's not an empty string */
	 strcpy(abuff,latexsym);	/* so get local copy */
       } /* --- end-of-if(strcmp(htmlsym,"&#")==0) --- */
      /* --- substitute macro arguments --- */
      if ( nargs > 0 )			/*substitute #1,#2,... in latexsym*/
       {
       char *arg1ptr = tokptr+escapelen;/* nargs begin after macro literal */
       char *optarg = args+1;		/* ptr 1 char past #args digit 0-9 */
       expptr = arg1ptr;		/* ptr to beginning of next arg */
       for ( iarg=1; iarg<=nargs; iarg++ ) /* one #`iarg` arg at a time */
	{
	char argsignal[32] = "#1",	/* #1...#9 signals arg replacement */
	*argsigptr = NULL;		/* ptr to argsignal in abuff[] */
	/* --- get argument value --- */
	*argval = '\000';		/* init arg as empty string */
	skipwhite(expptr);		/* and skip leading white space */
	if ( iarg==1 && *optarg!='\000'	/* check for optional [arg] */
	&&   !isalgebra )		/* but not in "algebra syntax" */
	 { strcpy(argval,optarg);	/* init with default value */
	   if ( *expptr == '[' )	/* but user gave us [argval] */
	     expptr = texsubexpr(expptr,argval,0,"[","]",0,0); } /*so get it*/
	else				/* not optional, so get {argval} */
	 if ( *expptr != '\000' ) {	/* check that some argval provided */
	   if ( !isalgebra )		/* only { } delims for latex macro */
	     expptr = texsubexpr(expptr,argval,0,"{","}",0,0);/*get {argval}*/
	   else {			/*any delim for algebra syntax macro*/
	     expptr = texsubexpr(expptr,argval,0,aleft,aright,0,1);
	     if ( isthischar(*argval,aleft) ) /* have delim-enclosed arg */
	       if ( *argval != '{' ) {	/* and it's not { }-enclosed */
	         strchange(0,argval,"\\left"); /* insert opening \left, */
	         strchange(0,argval+strlen(argval)-1,"\\right"); } }/*\right*/
	  } /* --- end-of-if(*expptr!='\000') --- */
	/* --- (recursively) call mimeprep() to prep the argument --- */
	if ( !isempty(argval) )		/* have an argument */
	  mimeprep(argval);		/* so (recursively) prep it */
	/* --- "specials" (e.g., \password) check --- */
	if ( strstr(htmlsym,"password") != NULL ) { /* have \password{} */
	  argval[64] = '\000';		/* make sure it's not too long */
	  strcpy(password,argval); }	/* save it for http_referer checks */
	/* --- replace #`iarg` in macro with argval --- */
	sprintf(argsignal,"#%d",iarg);	/* #1...#9 signals argument */
	while ( (argsigptr=strstr(argval,argsignal)) != NULL ) /* #1...#9 */
	 {strsqueeze(argsigptr,strlen(argsignal));} /* can't be in argval */
	while ( (argsigptr=strstr(abuff,argsignal)) != NULL ) /* #1...#9 */
	 strchange(strlen(argsignal),argsigptr,argval); /*replaced by argval*/
	} /* --- end-of-for(iarg) --- */
       escapelen += ((int)(expptr-arg1ptr)); /* add in length of all args */
       } /* --- end-of-if(nargs>0) --- */
      strchange(escapelen,tokptr,abuff); /*replace macro or html symbol*/
      expptr = tokptr + strlen(abuff); /*resume search after macro / html*/
      } /* --- end-of-while(tokptr!=NULL) --- */
  } /* --- end-of-for(isymbol) --- */
/* -------------------------------------------------------------------------
convert \left( to \(  and  \right) to \),  etc.
-------------------------------------------------------------------------- */
if ( xlateleft )			/* \left...\right xlation wanted */
 for ( idelim=0; idelim<2; idelim++ )	/* 0 for \left  and  1 for \right */
  {
  char	*lrstr  = (idelim==0?"\\left":"\\right"); /* \left on 1st pass */
  int	lrlen   = (idelim==0?5:6);	/* strlen() of \left or \right */
  char	*braces = (idelim==0?LEFTBRACES ".":RIGHTBRACES "."), /*([{<or)]}>*/
	**lrfrom= (idelim==0?leftfrom:rightfrom), /* long braces like \| */
	**lrto  = (idelim==0?leftto:rightto), /* xlated to 1-char like = */
	*lrsym  = NULL;			/* lrfrom[isymbol] */
  expptr = expression;			/* start search at beginning */
  while ( (tokptr=strstr(expptr,lrstr)) != NULL ) /* found \left or \right */
    {
    if ( isthischar(*(tokptr+lrlen),braces) ) /* followed by a 1-char brace*/
      {	strsqueeze((tokptr+1),(lrlen-1));/*so squeeze out "left" or "right"*/
	expptr = tokptr+2; }		/* and resume search past brace */
    else				/* may be a "long" brace like \| */
      {
      expptr = tokptr+lrlen;		/*init to resume search past\left\rt*/
      for(isymbol=0; (lrsym=lrfrom[isymbol]) != NULL; isymbol++)
	{ int symlen = strlen(lrsym);	/* #chars in delim, e.g., 2 for \| */
	  if ( memcmp(tokptr+lrlen,lrsym,symlen) == 0 ) /* found long delim*/
	    { strsqueeze((tokptr+1),(lrlen+symlen-2)); /*squeeze out delim*/
	      *(tokptr+1) = *(lrto[isymbol]); /* last char now 1-char delim*/
	      expptr = tokptr+2 - lrlen; /* resume search past 1-char delim*/
	      break; }			/* no need to check more lrsym's */
	} /* --- end-of-for(isymbol) --- */
      } /* --- end-of-if/else(isthischar()) --- */
    } /* --- end-of-while(tokptr!=NULL) --- */
  } /* --- end-of-for(idelim) --- */
/* -------------------------------------------------------------------------
run thru table, converting all {a+b\atop c+d} to \atop{a+b}{c+d}
-------------------------------------------------------------------------- */
for(isymbol=0; (atopsym=atopcommands[isymbol]) != NULL; isymbol++)
  {
  int	atoplen = strlen(atopsym);	/* #chars in \atop */
  expptr = expression;			/* re-start search at beginning */
  while ( (tokptr=strstr(expptr,atopsym)) != NULL ) /* found another atop */
    { char *leftbrace=NULL, *rightbrace=NULL; /*ptr to opening {, closing }*/
      char termchar = *(tokptr+atoplen); /* \atop followed by terminator */
      if ( msgfp!=NULL && msglevel>=999 )
	{ fprintf(msgfp,"mimeprep> offset=%d rhs=\"%s\"\n",
	  (int)(tokptr-expression),tokptr);
	  fflush(msgfp); }
      if ( isalpha((int)termchar) )	/*we just have prefix of longer sym*/
	{ expptr = tokptr+atoplen;	/* just resume search after prefix */
	  continue; }			/* but don't process it */
      leftbrace  = findbraces(expression,tokptr);     /* find left { */
      rightbrace = findbraces(NULL,tokptr+atoplen-1); /* find right } */
      if ( leftbrace==NULL || rightbrace==NULL )
	{ expptr += atoplen;  continue; } /* skip command if didn't find */
      else				/* we have bracketed { \atop } */
	{
	int  leftlen  = (int)(tokptr-leftbrace) - 1, /* #chars in left arg */
	     rightlen = (int)(rightbrace-tokptr) - atoplen, /* and in right*/
	     totlen   = (int)(rightbrace-leftbrace) + 1; /*tot in { \atop }*/
	char *open=atopdelims[2*isymbol], *close=atopdelims[2*isymbol+1];
	char arg[8192], command[8192];	/* left/right args, new \atop{}{} */
	*command = '\000';		/* start with null string */
	if (open!=NULL) strcat(command,open); /* add open delim if needed */
	strcat(command,atopsym);	/* add command with \atop */
	arg[0] = '{';			/* arg starts with { */
	memcpy(arg+1,leftbrace+1,leftlen); /* extract left-hand arg */
	arg[leftlen+1] = '\000';	/* and null terminate it */
	strcat(command,arg);		/* concatanate {left-arg to \atop */
	strcat(command,"}{");		/* close left-arg, open right-arg */
	memcpy(arg,tokptr+atoplen,rightlen); /* right-hand arg */
	arg[rightlen] = '}';		/* add closing } */
	arg[rightlen+1] = '\000';	/* and null terminate it */
	if ( isthischar(*arg,WHITEMATH) ) /* 1st char was mandatory space */
	  {strsqueeze(arg,1);}		/* so squeeze it out */
	strcat(command,arg);		/* concatanate right-arg} */
	if (close!=NULL) strcat(command,close); /* add close delim if needed*/
	strchange(totlen-2,leftbrace+1,command); /* {\atop} --> {\atop{}{}} */
	expptr = leftbrace+strlen(command); /*resume search past \atop{}{}*/
	}
    } /* --- end-of-while(tokptr!=NULL) --- */
  } /* --- end-of-for(isymbol) --- */
/* -------------------------------------------------------------------------
back to caller with preprocessed expression
-------------------------------------------------------------------------- */
if ( msgfp!=NULL && msglevel>=99 )	/* display preprocessed expression */
  { fprintf(msgfp,"mimeprep> expression=\"\"%s\"\"\n",expression);
    fflush(msgfp); }
return ( expression );
} /* --- end-of-function mimeprep() --- */


/* ==========================================================================
 * Function:	strchange ( int nfirst, char *from, char *to )
 * Purpose:	Changes the nfirst leading chars of `from` to `to`.
 *		For example, to change char x[99]="12345678" to "123ABC5678"
 *		call strchange(1,x+3,"ABC")
 * --------------------------------------------------------------------------
 * Arguments:	nfirst (I)	int containing #leading chars of `from`
 *				that will be replace by `to`
 *		from (I/O)	char * to null-terminated string whose nfirst
 *				leading chars will be replaced by `to`
 *		to (I)		char * to null-terminated string that will
 *				replace the nfirst leading chars of `from`
 * --------------------------------------------------------------------------
 * Returns:	( char * )	ptr to first char of input `from`
 *				or NULL for any error.
 * --------------------------------------------------------------------------
 * Notes:     o	If strlen(to)>nfirst, from must have memory past its null
 *		(i.e., we don't do a realloc)
 * ======================================================================= */
/* --- entry point --- */
char	*strchange ( int nfirst, char *from, char *to )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
int	tolen = (to==NULL?0:strlen(to)), /* #chars in replacement string */
	nshift = abs(tolen-nfirst);	/*need to shift from left or right*/
/* -------------------------------------------------------------------------
shift from left or right to accommodate replacement of its nfirst chars by to
-------------------------------------------------------------------------- */
if ( tolen < nfirst )			/* shift left is easy */
  {strsqueeze(from,nshift);}		/* memmove avoids overlap memory */
if ( tolen > nfirst )			/* need more room at start of from */
  { char *pfrom = from+strlen(from);	/* ptr to null terminating from */
    for ( ; pfrom>=from; pfrom-- )	/* shift all chars including null */
      *(pfrom+nshift) = *pfrom; }	/* shift chars nshift places right */
/* -------------------------------------------------------------------------
from has exactly the right number of free leading chars, so just put to there
-------------------------------------------------------------------------- */
if ( tolen != 0 )			/* make sure to not empty or null */
  memcpy(from,to,tolen);		/* chars moved into place */
return ( from );			/* changed string back to caller */
} /* --- end-of-function strchange() --- */


/* ==========================================================================
 * Function:	strreplace (char *string, char *from, char *to, int nreplace)
 * Purpose:	Changes the first nreplace occurrences of 'from' to 'to'
 *		in string, or all occurrences if nreplace=0.
 * --------------------------------------------------------------------------
 * Arguments:	string (I/0)	char * to null-terminated string in which
 *				occurrence of 'from' will be replaced by 'to'
 *		from (I)	char * to null-terminated string
 *				to be replaced by 'to'
 *		to (I)		char * to null-terminated string that will
 *				replace 'from'
 *		nreplace (I)	int containing (maximum) number of
 *				replacements, or 0 to replace all.
 * --------------------------------------------------------------------------
 * Returns:	( int )		number of replacements performed,
 *				or 0 for no replacements or -1 for any error.
 * --------------------------------------------------------------------------
 * Notes:     o
 * ======================================================================= */
/* --- entry point --- */
int	strreplace ( char *string, char *from, char *to, int nreplace )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
int	fromlen = (from==NULL?0:strlen(from)), /* #chars to be replaced */
	tolen = (to==NULL?0:strlen(to)); /* #chars in replacement string */
char	*pfrom = (char *)NULL,		/*ptr to 1st char of from in string*/
	*pstring = string,		/*ptr past previously replaced from*/
	*strchange();			/* change 'from' to 'to' */
int	nreps = 0;			/* #replacements returned to caller*/
/* -------------------------------------------------------------------------
repace occurrences of 'from' in string to 'to'
-------------------------------------------------------------------------- */
if ( string == (char *)NULL		/* no input string */
||   (fromlen<1 && nreplace<=0) )	/* replacing empty string forever */
  nreps = (-1);				/* so signal error */
else					/* args okay */
  while (nreplace<1 || nreps<nreplace)	/* up to #replacements requested */
    {
    if ( fromlen > 0 )			/* have 'from' string */
      pfrom = strstr(pstring,from);	/*ptr to 1st char of from in string*/
    else  pfrom = pstring;		/*or empty from at start of string*/
    if ( pfrom == (char *)NULL ) break;	/*no more from's, so back to caller*/
    if ( strchange(fromlen,pfrom,to)	/* leading 'from' changed to 'to' */
    ==   (char *)NULL ) { nreps=(-1); break; } /* signal error to caller */
    nreps++;				/* count another replacement */
    pstring = pfrom+tolen;		/* pick up search after 'to' */
    if ( *pstring == '\000' ) break;	/* but quit at end of string */
    } /* --- end-of-while() --- */
return ( nreps );			/* #replacements back to caller */
} /* --- end-of-function strreplace() --- */


/* ==========================================================================
 * Function:	strwstr (char *string, char *substr, char *white, int *sublen)
 * Purpose:	Find first substr in string, but wherever substr contains
 *		a whitespace char (in white), string may contain any number
 *		(including 0) of whitespace chars. If white contains I or i,
 *		then match is case-insensitive (and I,i _not_ whitespace).
 * --------------------------------------------------------------------------
 * Arguments:	string (I)	char * to null-terminated string in which
 *				first occurrence of substr will be found
 *		substr (I)	char * to null-terminated string containing
 *				"template" that will be searched for
 *		white (I)	char * to null-terminated string containing
 *				whitespace chars.  If NULL or empty, then
 *				"~ \t\n\r\f\v" (WHITEMATH in mimetex.h) used.
 *				If white contains I or i, then match is
 *				case-insensitive (and I,i _not_ considered
 *				whitespace).
 *		sublen (O)	address of int returning "length" of substr
 *				found in string (which may be longer or
 *				shorter than substr itself).
 * --------------------------------------------------------------------------
 * Returns:	( char * )	ptr to first char of substr in string
 *				or NULL if not found or for any error.
 * --------------------------------------------------------------------------
 * Notes:     o	Wherever a single whitespace char appears in substr,
 *		the corresponding position in string may contain any
 *		number (including 0) of whitespace chars, e.g.,
 *		string="abc   def" and string="abcdef" both match
 *		substr="c d" at offset 2 of string.
 *	      o	If substr="c  d" (two spaces between c and d),
 *		then string must have at least one space, so now "abcdef"
 *		doesn't match.  In general, the minimum number of spaces
 *		in string is the number of spaces in substr minus 1
 *		(so 1 space in substr permits 0 spaces in string).
 *	      o	Embedded spaces are counted in sublen, e.g.,
 *		string="c   d" (three spaces) matches substr="c d"
 *		with sublen=5 returned.  But string="ab   c   d" will
 *		also match substr="  c d" returning sublen=5 and
 *		a ptr to the "c".  That is, the mandatory preceding
 *		space is _not_ counted as part of the match.
 *		But all the embedded space is counted.
 *		(An inconsistent bug/feature is that mandatory
 *		terminating space is counted.)
 *	      o	Moreover, string="c   d" matches substr="  c d", i.e.,
 *		the very beginning of a string is assumed to be preceded
 *		by "virtual blanks".
 * ======================================================================= */
/* --- entry point --- */
char	*strwstr ( char *string, char *substr, char *white, int *sublen )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
char	*psubstr=substr, *pstring=string,/*ptr to current char in substr,str*/
	*pfound = (char *)NULL;		/*ptr to found substr back to caller*/
char	*pwhite=NULL, whitespace[256];	/* callers white whithout i,I */
int	iscase = (white==NULL?1:	/* case-sensitive if i,I in white */
	strchr(white,'i')==NULL && strchr(white,'I')==NULL);
int	foundlen = 0;			/* length of substr found in string*/
int	nstrwhite=0, nsubwhite=0,	/* #leading white chars in str,sub */
	nminwhite=0;			/* #mandatory leading white in str */
int	nstrchars=0, nsubchars=0,	/* #non-white chars to be matched */
	isncmp=0;			/*strncmp() or strncasecmp() result*/
/* -------------------------------------------------------------------------
Initialization
-------------------------------------------------------------------------- */
/* --- set up whitespace --- */
strcpy(whitespace,WHITEMATH);		/*default if no user input for white*/
if ( white != NULL )			/*user provided ptr to white string*/
 if ( *white != '\000' ) {		/*and it's not just an empty string*/
   strcpy(whitespace,white);		/* so use caller's white spaces */
   while ( (pwhite=strchr(whitespace,'i')) != NULL ) /* have an embedded i */
     {strsqueeze(pwhite,1);}		/* so squeeze it out */
   while ( (pwhite=strchr(whitespace,'I')) != NULL ) /* have an embedded I */
     {strsqueeze(pwhite,1);}		/* so squeeze it out */
   if ( *whitespace == '\000' )		/* caller's white just had i,I */
     strcpy(whitespace,WHITEMATH); }	/* so revert back to default */
/* -------------------------------------------------------------------------
Find first occurrence of substr in string
-------------------------------------------------------------------------- */
if ( string != NULL )			/* caller passed us a string ptr */
 while ( *pstring != '\000' ) {		/* break when string exhausted */
  char	*pstrptr = pstring;		/* (re)start at next char in string*/
  int	leadingwhite = 0;		/* leading whitespace */
  psubstr = substr;			/* start at beginning of substr */
  foundlen = 0;				/* reset length of found substr */
  if ( substr != NULL )			/* caller passed us a substr ptr */
   while ( *psubstr != '\000' ) {	/*see if pstring begins with substr*/
    /* --- check for end-of-string before finding match --- */
    if ( *pstrptr == '\000' )		/* end-of-string without a match */
      goto nextstrchar;			/* keep trying with next char */
    /* --- actual amount of whitespace in string and substr --- */
    nsubwhite = strspn(psubstr,whitespace); /* #leading white chars in sub */
    nstrwhite = strspn(pstrptr,whitespace); /* #leading white chars in str */
    nminwhite = max2(0,nsubwhite-1);	/* #mandatory leading white in str */
    /* --- check for mandatory leading whitespace in string --- */
    if ( pstrptr != string )		/*not mandatory at start of string*/
      if ( nstrwhite < nminwhite )	/* too little leading white space */
	goto nextstrchar;		/* keep trying with next char */
    /* ---hold on to #whitespace chars in string preceding substr match--- */
    if ( pstrptr == pstring )		/* whitespace at start of substr */
      leadingwhite = nstrwhite;		/* save it as leadingwhite */
    /* --- check for optional whitespace --- */
    if ( psubstr != substr )		/* always okay at start of substr */
      if ( nstrwhite>0 && nsubwhite<1 )	/* too much leading white space */
	goto nextstrchar;		/* keep trying with next char */
    /* --- skip any leading whitespace in substr and string --- */
    psubstr += nsubwhite;		/* push past leading sub whitespace*/
    pstrptr += nstrwhite;		/* push past leading str whitespace*/
    /* --- now get non-whitespace chars that we have to match --- */
    nsubchars = strcspn(psubstr,whitespace); /* #non-white chars in sub */
    nstrchars = strcspn(pstrptr,whitespace); /* #non-white chars in str */
    if ( nstrchars < nsubchars )	/* too few chars for match */
      goto nextstrchar;			/* keep trying with next char */
    /* --- see if next nsubchars are a match --- */
    isncmp = (iscase? strncmp(pstrptr,psubstr,nsubchars): /*case sensitive*/
		strncasecmp(pstrptr,psubstr,nsubchars)); /*case insensitive*/
    if ( isncmp != 0 )			/* no match */
      goto nextstrchar;			/* keep trying with next char */
    /* --- push past matched chars --- */
    psubstr += nsubchars;  pstrptr += nsubchars;  /*nsubchars were matched*/
    } /* --- end-of-while(*psubstr!='\000') --- */
  pfound = pstring + leadingwhite;	/* found match starting at pstring */
  foundlen = (int)(pstrptr-pfound);	/* consisting of this many chars */
  goto end_of_job;			/* back to caller */
  /* ---failed to find substr, continue trying with next char in string--- */
  nextstrchar:				/* continue outer loop */
    pstring++;				/* bump to next char in string */
  } /* --- end-of-while(*pstring!='\000') --- */
/* -------------------------------------------------------------------------
Back to caller with ptr to first occurrence of substr in string
-------------------------------------------------------------------------- */
end_of_job:
  if ( msglevel>=999 && msgfp!=NULL) {	/* debugging/diagnostic output */
    fprintf(msgfp,"strwstr> str=\"%.72s\" sub=\"%s\" found at offset %d\n",
    string,substr,(pfound==NULL?(-1):(int)(pfound-string))); fflush(msgfp); }
  if ( sublen != NULL )			/*caller wants length of found substr*/
    *sublen = foundlen;			/* give it to him along with ptr */
  return ( pfound );			/*ptr to first found substr, or NULL*/
} /* --- end-of-function strwstr() --- */


/* ==========================================================================
 * Function:	strdetex ( s, mode )
 * Purpose:	Removes/replaces any LaTeX math chars in s
 *		so that s can be displayed "verbatim",
 *		e.g., for error messages.
 * --------------------------------------------------------------------------
 * Arguments:	s (I)		char * to null-terminated string
 *				whose math chars are to be removed/replaced
 *		mode (I)	int containing 0 to _not_ use macros (i.e.,
 *				mimeprep won't be called afterwards),
 *				or containing 1 to use macros that will
 *				be expanded by a subsequent call to mimeprep.
 * --------------------------------------------------------------------------
 * Returns:	( char * )	ptr to "cleaned" copy of s
 *				or "" (empty string) for any error.
 * --------------------------------------------------------------------------
 * Notes:     o	The returned pointer addresses a static buffer,
 *		so don't call strdetex() again until you're finished
 *		with output from the preceding call.
 * ======================================================================= */
/* --- entry point --- */
char	*strdetex ( char *s, int mode )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
static	char sbuff[4096];		/* copy of s with no math chars */
int	strreplace();			/* replace _ with -, etc */
/* -------------------------------------------------------------------------
Make a clean copy of s
-------------------------------------------------------------------------- */
/* --- check input --- */
*sbuff = '\000';			/* initialize in case of error */
if ( isempty(s) ) goto end_of_job;	/* no input */
/* --- start with copy of s --- */
strninit(sbuff,s,2048);			/* leave room for replacements */
/* --- make some replacements -- we *must* replace \ { } first --- */
strreplace(sbuff,"\\","\\backslash~\\!\\!",0);	/*change all \'s to text*/
strreplace(sbuff,"{", "\\lbrace~\\!\\!",0);	/*change all {'s to \lbrace*/
strreplace(sbuff,"}", "\\rbrace~\\!\\!",0);	/*change all }'s to \rbrace*/
/* --- now our further replacements may contain \directives{args} --- */
if( mode >= 1 ) strreplace(sbuff,"_","\\_",0);	/* change all _'s to \_ */
else strreplace(sbuff,"_","\\underline{\\qquad}",0); /*change them to text*/
if(0)strreplace(sbuff,"<","\\textlangle ",0);	/* change all <'s to text */
if(0)strreplace(sbuff,">","\\textrangle ",0);	/* change all >'s to text */
if(0)strreplace(sbuff,"$","\\textdollar ",0);	/* change all $'s to text */
strreplace(sbuff,"$","\\$",0);			/* change all $'s to \$ */
strreplace(sbuff,"&","\\&",0);			/* change all &'s to \& */
strreplace(sbuff,"%","\\%",0);			/* change all %'s to \% */
strreplace(sbuff,"#","\\#",0);			/* change all #'s to \# */
/*strreplace(sbuff,"~","\\~",0);*/		/* change all ~'s to \~ */
strreplace(sbuff,"^","{\\fs{+2}\\^}",0);	/* change all ^'s to \^ */
end_of_job:
  return ( sbuff );			/* back with clean copy of s */
} /* --- end-of-function strdetex() --- */


/* ==========================================================================
 * Function:	strtexchr ( char *string, char *texchr )
 * Purpose:	Find first texchr in string, but texchr must be followed
 *		by non-alpha
 * --------------------------------------------------------------------------
 * Arguments:	string (I)	char * to null-terminated string in which
 *				first occurrence of delim will be found
 *		texchr (I)	char * to null-terminated string that
 *				will be searched for
 * --------------------------------------------------------------------------
 * Returns:	( char * )	ptr to first char of texchr in string
 *				or NULL if not found or for any error.
 * --------------------------------------------------------------------------
 * Notes:     o	texchr should contain its leading \, e.g., "\\left"
 * ======================================================================= */
/* --- entry point --- */
char	*strtexchr ( char *string, char *texchr )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
char	delim, *ptexchr=(char *)NULL;	/* ptr returned to caller*/
char	*pstring = string;		/* start or continue up search here*/
int	texchrlen = (texchr==NULL?0:strlen(texchr)); /* #chars in texchr */
/* -------------------------------------------------------------------------
locate texchr in string
-------------------------------------------------------------------------- */
if ( string != (char *)NULL		/* check that we got input string */
&&   texchrlen > 0 ) {			/* and a texchr to search for */
 while ( (ptexchr=strstr(pstring,texchr)) /* look for texchr in string */
 != (char *)NULL )			/* found it */
  if ( (delim = ptexchr[texchrlen])	/* char immediately after texchr */
  ==   '\000' ) break;			/* texchr at very end of string */
  else					/* if there are chars after texchr */
   if ( isalpha(delim)			/*texchr is prefix of longer symbol*/
   ||   0 )				/* other tests to be determined */
    pstring = ptexchr + texchrlen;	/* continue search after texchr */
   else					/* passed all tests */
    break; }				/*so return ptr to texchr to caller*/
return ( ptexchr );			/* ptr to texchar back to caller */
} /* --- end-of-function strtexchr() --- */


/* ==========================================================================
 * Function:	findbraces ( char *expression, char *command )
 * Purpose:	If expression!=NULL, finds opening left { preceding command;
 *		if expression==NULL, finds closing right } after command.
 *		For example, to parse out {a+b\over c+d} call findbraces()
 *		twice.
 * --------------------------------------------------------------------------
 * Arguments:	expression (I)	NULL to find closing right } after command,
 *				or char * to null-terminated string to find
 *				left opening { preceding command.
 *		command (I)	char * to null-terminated string whose
 *				first character is usually the \ of \command
 * --------------------------------------------------------------------------
 * Returns:	( char * )	ptr to either opening { or closing },
 *				or NULL for any error.
 * --------------------------------------------------------------------------
 * Notes:     o
 * ======================================================================= */
/* --- entry point --- */
char	*findbraces ( char *expression, char *command )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
int	isopen = (expression==NULL?0:1); /* true to find left opening { */
char	*left="{", *right="}",		/* delims bracketing {x\command y} */
	*delim = (isopen?left:right),	/* delim we want,  { if isopen */
	*match = (isopen?right:left),	/* matching delim, } if isopen */
	*brace = NULL;			/* ptr to delim returned to caller */
int	inc = (isopen?-1:+1);		/* pointer increment */
int	level = 1;			/* nesting level, for {{}\command} */
char	*ptr = command;			/* start search here */
int	setbrace = 1;			/* true to set {}'s if none found */
/* -------------------------------------------------------------------------
search for left opening { before command, or right closing } after command
-------------------------------------------------------------------------- */
while ( 1 )				/* search for brace, or until end */
  {
  /* --- next char to check for delim --- */
  ptr += inc;				/* bump ptr left or right */
  /* --- check for beginning or end of expression --- */
  if ( isopen )				/* going left, check for beginning */
       { if ( ptr < expression ) break;	} /* went before start of string */
  else { if ( *ptr == '\000' ) break; }	/* went past end of string */
  /* --- don't check this char if it's escaped --- */
  if ( !isopen || ptr>expression )	/* very first char can't be escaped*/
    if ( isthischar(*(ptr-1),ESCAPE) )	/* escape char precedes current */
      continue;				/* so don't check this char */
  /* --- check for delim --- */
  if ( isthischar(*ptr,delim) )		/* found delim */
    if ( --level == 0 )			/* and it's not "internally" nested*/
      {	brace = ptr;			/* set ptr to brace */
	goto end_of_job; }		/* and return it to caller */
  /* --- check for matching delim --- */
  if ( isthischar(*ptr,match) )		/* found matching delim */
    level++;				/* so bump nesting level */
  } /* --- end-of-while(1) --- */
end_of_job:
  if ( brace == (char *)NULL )		/* open{ or close} not found */
    if ( setbrace )			/* want to force one at start/end? */
      brace = ptr;			/* { before expressn, } after cmmnd*/
  return ( brace );			/*back to caller with delim or NULL*/
} /* --- end-of-function findbraces() --- */


#if 0  /* --- enhanced version from mathtex below --- */
/* ==========================================================================
 * Function:	strpspn ( char *s, char *reject, char *segment )
 * Purpose:	finds the initial segment of s containing no chars
 *		in reject that are outside (), [] and {} parens, e.g.,
 *		   strpspn("abc(---)def+++","+-",segment) returns
 *		   segment="abc(---)def" and a pointer to the first '+' in s
 *		because the -'s are enclosed in () parens.
 * --------------------------------------------------------------------------
 * Arguments:	s (I)		(char *)pointer to null-terminated string
 *				whose initial segment is desired
 *		reject (I)	(char *)pointer to null-terminated string
 *				containing the "reject chars"
 *		segment (O)	(char *)pointer returning null-terminated
 *				string comprising the initial segment of s
 *				that contains non-rejected chars outside
 *				(),[],{} parens, i.e., all the chars up to
 *				but not including the returned pointer.
 *				(That's the entire string if no non-rejected
 *				chars are found.)
 * --------------------------------------------------------------------------
 * Returns:	( char * )	pointer to first reject-char found in s
 *				outside parens, or a pointer to the
 *				terminating '\000' of s if there are
 *				no reject chars in s outside all () parens.
 * --------------------------------------------------------------------------
 * Notes:     o	the return value is _not_ like strcspn()'s
 *	      o	improperly nested (...[...)...] are not detected,
 *		but are considered "balanced" after the ]
 *	      o	if reject not found, segment returns the entire string s
 *	      o	leading/trailing whitespace is trimmed from returned segment
 * ======================================================================= */
/* --- entry point --- */
char	*strpspn ( char *s, char *reject, char *segment )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
char	*ps = s;			/* current pointer into s */
int	depth = 0;			/* () paren nesting level */
int	seglen=0, maxseg=2047;		/* segment length, max allowed */
/* -------------------------------------------------------------------------
initialization
-------------------------------------------------------------------------- */
/* --- check arguments --- */
if ( isempty(s) )			/* no input string supplied */
  goto end_of_job;			/* no reject chars supplied */
/* -------------------------------------------------------------------------
find first char from s outside () parens (and outside ""'s) and in reject
-------------------------------------------------------------------------- */
while ( *ps != '\000' ) {		/* search till end of input string */
  if ( isthischar(*ps,"([{") ) depth++;	/* push another paren */
  if ( isthischar(*ps,")]}") ) depth--;	/* or pop another paren */
  if ( depth < 1 ) {			/* we're outside all parens */
    if ( isempty(reject) ) break;	/* no reject so break immediately */
    if ( isthischar(*ps,reject) ) break; } /* only break on a reject char */
  if ( segment != NULL )		/* caller gave us segment */
    if ( seglen < maxseg )		/* don't overflow segment buffer */
      memcpy(segment+seglen,ps,1);	/* so copy non-reject char */
  seglen += 1;  ps += 1;		/* bump to next char */
  } /* --- end-of-while(*ps!=0) --- */
end_of_job:
  if ( segment != NULL ) {		/* caller gave us segment */
    if ( isempty(reject) ) {		/* no reject char */
      segment[min2(seglen,maxseg)] = *ps;  seglen++; } /*closing )]} to seg*/
    segment[min2(seglen,maxseg)] = '\000'; /* null-terminate the segment */
    trimwhite(segment); }		/* trim leading/trailing whitespace*/
  return ( ps );			/* back to caller */
} /* --- end-of-function strpspn() --- */
#endif


/* ==========================================================================
 * Function:	isstrstr ( char *string, char *snippets, int iscase )
 * Purpose:	determine whether any substring of 'string'
 *		matches any of the comma-separated list of 'snippets',
 *		ignoring case if iscase=0.
 * --------------------------------------------------------------------------
 * Arguments:	string (I)	char * containing null-terminated
 *				string that will be searched for
 *				any one of the specified snippets
 *		snippets (I)	char * containing null-terminated,
 *				comma-separated list of snippets
 *				to be searched for in string
 *		iscase (I)	int containing 0 for case-insensitive
 *				comparisons, or 1 for case-sensitive
 * --------------------------------------------------------------------------
 * Returns:	( int )		1 if any snippet is a substring of
 *				string, 0 if not
 * --------------------------------------------------------------------------
 * Notes:     o
 * ======================================================================= */
/* --- entry point --- */
int	isstrstr ( char *string, char *snippets, int iscase )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
int	status = 0;			/*1 if any snippet found in string*/
char	snip[256], *snipptr = snippets,	/* munge through each snippet */
	delim = ',', *delimptr = NULL;	/* separated by delim's */
char	stringcp[4096], *cp = stringcp;	/*maybe lowercased copy of string*/
/* -------------------------------------------------------------------------
initialization
-------------------------------------------------------------------------- */
/* --- arg check --- */
if ( string==NULL || snippets==NULL ) goto end_of_job; /* missing arg */
if ( *string=='\000' || *snippets=='\000' ) goto end_of_job; /* empty arg */
/* --- copy string and lowercase it if case-insensitive --- */
strninit(stringcp,string,4064);		/* local copy of string */
if ( !iscase )				/* want case-insensitive compares */
  for ( cp=stringcp; *cp != '\000'; cp++ ) /* so for each string char */
    if ( isupper(*cp) ) *cp = tolower(*cp); /*lowercase any uppercase chars*/
/* -------------------------------------------------------------------------
extract each snippet and see if it's a substring of string
-------------------------------------------------------------------------- */
while ( snipptr != NULL )		/* while we still have snippets */
  {
  /* --- extract next snippet --- */
  if ( (delimptr = strchr(snipptr,delim)) /* locate next comma delim */
  ==   NULL )				/*not found following last snippet*/
    { strninit(snip,snipptr,255);	/* local copy of last snippet */
      snipptr = NULL; }			/* signal end-of-string */
  else					/* snippet ends just before delim */
    { int sniplen = (int)(delimptr-snipptr) - 1;  /* #chars in snippet */
      memcpy(snip,snipptr,sniplen);	/* local copy of snippet chars */
      snip[sniplen] = '\000';		/* null-terminated snippet */
      snipptr = delimptr + 1; }		/* next snippet starts after delim */
  /* --- lowercase snippet if case-insensitive --- */
  if ( !iscase )			/* want case-insensitive compares */
    for ( cp=snip; *cp != '\000'; cp++ ) /* so for each snippet char */
      if ( isupper(*cp) ) *cp=tolower(*cp); /*lowercase any uppercase chars*/
  /* --- check if snippet in string --- */
  if ( strstr(stringcp,snip) != NULL )	/* found snippet in string */
    { status = 1;			/* so reset return status */
      break; }				/* no need to check any further */
  } /* --- end-of-while(*snipptr!=0) --- */
end_of_job: return ( status );		/*1 if snippet found in list, else 0*/
} /* --- end-of-function isstrstr() --- */


/* ==========================================================================
 * Function:	isnumeric ( s )
 * Purpose:	determine if s is an integer
 * --------------------------------------------------------------------------
 * Arguments:	s (I)		(char *)pointer to null-terminated string
 *				that's checked for a leading + or -
 *				followed by digits
 * --------------------------------------------------------------------------
 * Returns:	( int )		1 if s is numeric, 0 if it is not
 * --------------------------------------------------------------------------
 * Notes:     o
 * ======================================================================= */
/* --- entry point --- */
int	isnumeric ( char *s )
{
/* -------------------------------------------------------------------------
determine whether s is an integer
-------------------------------------------------------------------------- */
int	status = 0;			/* return 0 if not numeric, 1 if is*/
char	*p = s;				/* pointer into s */
if ( isempty(s) ) goto end_of_job;	/* missing arg or empty string */
skipwhite(p);				/*check for leading +or- after space*/
if ( *p=='+' || *p=='-' ) p++;		/* skip leading + or - */
for ( ; *p != '\000'; p++ ) {		/* check rest of s for digits */
  if ( isdigit(*p) ) continue;		/* still got uninterrupted digits */
  if ( !isthischar(*p,WHITESPACE) ) goto end_of_job; /* non-numeric char */
  skipwhite(p);				/* skip all subsequent whitespace */
  if ( *p == '\000' ) break;		/* trailing whitespace okay */
  goto end_of_job;			/* embedded whitespace non-numeric */
  } /* --- end-of-for(*p) --- */
status = 1;				/* numeric after checks succeeded */
end_of_job:
  return ( status );			/*back to caller with 1=string, 0=no*/
} /* --- end-of-function isnumeric() --- */


/* ==========================================================================
 * Function:	evalterm ( STORE *store, char *term )
 * Purpose:	evaluates a term
 * --------------------------------------------------------------------------
 * Arguments:	store (I/O)	STORE * containing environment
 *				in which term is to be evaluated
 *		term (I)	char * containing null-terminated string
 *				with a term like "3" or "a" or "a+3"
 *				whose value is to be determined
 * --------------------------------------------------------------------------
 * Returns:	( int )		value of term,
 *				or NOVALUE for any error
 * --------------------------------------------------------------------------
 * Notes:     o	Also evaluates index?a:b:c:etc, returning a if index<=0,
 *		b if index=1, etc, and the last value if index is too large.
 *		Each a:b:c:etc can be another expression, including another
 *		(index?a:b:c:etc) which must be enclosed in parentheses.
 * ======================================================================= */
/* --- entry point --- */
int	evalterm ( STORE *store, char *term )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
int	termval = 0;			/* term value returned to caller */
char	token[2048] = "\000",		/* copy term */
	*delim = NULL;			/* delim '(' or '?' in token */
/*int	evalwff(),*/			/* recurse to evaluate terms */
/*	evalfunc();*/			/* evaluate function(arg1,arg2,...)*/
char	*strpspn();			/* span delims */
int	getstore();			/* lookup variables */
int	isnumeric();			/* numeric=constant, else variable */
static	int evaltermdepth = 0;		/* recursion depth */
int	novalue = (-89123456);		/* dummy (for now) error signal */
/* -------------------------------------------------------------------------
Initialization
-------------------------------------------------------------------------- */
if ( ++evaltermdepth > 99 ) goto end_of_job; /*probably recursing forever*/
if ( store==NULL || isempty(term) ) goto end_of_job; /*check for missing arg*/
skipwhite(term);			/* skip any leading whitespace */
/* -------------------------------------------------------------------------
First look for conditional of the form term?term:term:...
-------------------------------------------------------------------------- */
/* ---left-hand part of conditional is chars preceding "?" outside ()'s--- */
delim = strpspn(term,"?",token);	/* chars preceding ? outside () */
if ( *delim != '\000' ) {		/* found conditional expression */
  int ncolons = 0;			/* #colons we've found so far */
  if ( *token != '\000' )		/* evaluate "index" value on left */
    if ( (termval=evalterm(store,token)) /* evaluate left-hand term */
    == novalue ) goto end_of_job;	/* return error if failed */
  while ( *delim != '\000' ) {		/* still have chars in term */
    delim++; *token='\000';		/* initialize for next "value:" */
    if ( *delim == '\000' ) break;	/* no more values */
    delim = strpspn(delim,":",token);	/* chars preceding : outside () */
    if ( ncolons++ >= termval ) break;	/* have corresponding term */
    } /* --- end-of-while(*delim!='\000')) --- */
  if ( *token != '\000' )		/* have x:x:value:x:x on right */
    termval=evalterm(store,token);	/* so evaluate it */
  goto end_of_job;			/* return result to caller */
  } /* --- end-of-if(*delim!='\000')) --- */
/* -------------------------------------------------------------------------
evaluate a+b recursively
-------------------------------------------------------------------------- */
/* --- left-hand part of term is chars preceding "/+-*%" outside ()'s --- */
term = strpspn(term,"/+-*%",token);	/* chars preceding /+-*% outside ()*/
/* --- evaluate a+b, a-b, etc --- */
if ( *term != '\000' ) {		/* found arithmetic operation */
  int leftval=0, rightval=0;		/* init leftval for unary +a or -a */
  if ( *token != '\000' )		/* or eval for binary a+b or a-b */
    if ( (leftval=evalterm(store,token)) /* evaluate left-hand term */
    == novalue ) goto end_of_job;	/* return error if failed */
  if ( (rightval=evalterm(store,term+1)) /* evaluate right-hand term */
  == novalue ) goto end_of_job;		/* return error if failed */
  switch ( *term ) {			/* perform requested arithmetic */
    default: break;			/* internal error */
    case '+': termval = leftval+rightval;  break;  /* addition */
    case '-': termval = leftval-rightval;  break;  /* subtraction */
    case '*': termval = leftval*rightval;  break;  /* multiplication */
    case '/': if ( rightval != 0 )	/* guard against divide by zero */
                termval = leftval/rightval;  break; /* integer division */
    case '%': if ( rightval != 0 )	/* guard against divide by zero */
                termval = leftval%rightval;  break; /*left modulo right */
    } /* --- end-of-switch(*relation) --- */
  goto end_of_job;			/* return result to caller */
  } /* --- end-of-if(*term!='\000')) --- */
/* -------------------------------------------------------------------------
check for parenthesized expression or term of the form function(arg1,arg2,...)
-------------------------------------------------------------------------- */
if ( (delim = strchr(token,'(')) != NULL ) { /* token contains a ( */
  /* --- strip trailing paren (if there hopefully is one) --- */
  int  toklen = strlen(token);		/* total #chars in token */
  if ( token[toklen-1] == ')' )		/* found matching ) at end of token*/
    token[--toklen] = '\000';		/* remove trailing ) */
  /* --- handle parenthesized subexpression --- */
  if ( *token == '(' ) {		/* have parenthesized expression */
    strsqueeze(token,1);		/* so squeeze out leading ( */
    /* --- evaluate edited term --- */
    trimwhite(token);			/* trim leading/trailing whitespace*/
    termval = evalterm(store,token); }	/* evaluate token recursively */
  /* --- handle function(arg1,arg2,...) --- */
  else {				/* have function(arg1,arg2,...) */
    *delim = '\000';			/* separate function name and args */
    /*termval = evalfunc(store,token,delim+1);*/ } /* evaluate function */
  goto end_of_job; }			/* return result to caller */
/* -------------------------------------------------------------------------
evaluate constants directly, or recursively evaluate variables, etc
-------------------------------------------------------------------------- */
if ( *token != '\000' ) {		/* empty string */
  if ( isnumeric(token) )		/* have a constant */
    termval = atoi(token);		/* convert ascii-to-int */
  else {				/* variable or "stored proposition"*/
    termval = getstore(store,token); }	/* look up token */
  } /* --- end-of-if(*token!=0) --- */
/* -------------------------------------------------------------------------
back to caller with truth value of proposition
-------------------------------------------------------------------------- */
end_of_job:
  /* --- back to caller --- */
  if ( evaltermdepth > 0 ) evaltermdepth--;  /* pop recursion depth */
  return ( termval );			/* back to caller with value */
} /* --- end-of-function evalterm() --- */


/* ==========================================================================
 * Function:	getstore ( store, identifier )
 * Purpose:	finds identifier in store and returns corresponding value
 * --------------------------------------------------------------------------
 * Arguments:	store (I)	(STORE *)pointer to store containing
 *				the desired identifier
 *		identifier (I)	(char *)pointer to null-terminated string
 *				containing the identifier whose value
 *				is to be returned
 * --------------------------------------------------------------------------
 * Returns:	( int )		identifier's corresponding value,
 *				or 0 if identifier not found (or any error)
 * --------------------------------------------------------------------------
 * Notes:     o
 * ======================================================================= */
/* --- entry point --- */
int	getstore ( STORE *store, char *identifier )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
int	value = 0;		/* store[istore].value for identifier */
int	istore=0;		/* store[] index containing identifier */
char	seek[512], hide[512];	/* identifier arg, identifier in store */
/* --- first check args --- */
if ( store==NULL || isempty(identifier)) goto end_of_job; /* missing arg */
strninit(seek,identifier,500);	/* local copy of caller's identifier */
trimwhite(seek);		/* remove leading/trailing whitespace */
/* --- loop over store --- */
for ( istore=0; istore<MAXSTORE; istore++ ) { /* until end-of-table */
  char *idstore = store[istore].identifier; /* ptr to identifier in store */
  if ( isempty(idstore) )	/* empty id signals eot */
    break;			/* de-reference any default/error value */
  strninit(hide,idstore,500);	/* local copy of store[] identifier */
  trimwhite(hide);		/* remove leading/trailing whitespace */
  if ( !strcmp(hide,seek) )	/* found match */
    break;			/* de-reference corresponding value */
  } /* --- end-of-for(istore) --- */
if ( store[istore].value != NULL ) /* address of int supplied */
  value = *(store[istore].value);  /* return de-referenced int */
end_of_job:
  return ( value );			/* store->values[istore] or NULL */
} /* --- end-of-function getstore() --- */


/* ==========================================================================
 * Function:	getdirective(string, directive, iscase, isvalid, nargs, args)
 * Purpose:	Locates the first \directive{arg1}...{nargs} in string,
 *		returns arg1...nargs in args[],
 *		and removes \directive and its args from string.
 * --------------------------------------------------------------------------
 * Arguments:	string (I/0)	char * to null-terminated string from which
 *				the first occurrence of \directive will be
 *				interpreted and removed
 *		directive (I)	char * to null-terminated string containing
 *				the \directive to be interpreted in string
 *		iscase (I)	int containing 1 if match of \directive
 *				in string should be case-sensitive,
 *				or 0 if match is case-insensitive.
 *		isvalid (I)	int containing validity check option:
 *				0=no checks, 1=must be numeric
 *		nargs (I)	int containing (maximum) number of
 *				{args} following \directive, or 0 if none.
 *		args (O)	void * interpreted as (char *) if nargs=1
 *				to return the one and only arg,
 *				or interpreted as (char **) if nargs>1
 *				to array of returned arg strings
 * --------------------------------------------------------------------------
 * Returns:	( char * )	ptr to first char after removed \directive, or
 *				NULL if \directive not found, or any error.
 * --------------------------------------------------------------------------
 * Notes:     o	If optional [arg]'s are found, they're stored in the global
 *		optionalargs[] buffer, and the noptional counter is bumped.
 *	      o	set global argformat's decimal digits for each arg,
 *		e.g., 1357... means 1 for 1st arg, 3 for 2nd, 5 for 3rd, etc.
 *		0 for an arg is the default format (i.e., argformat=0),
 *		and means it's formatted as a LaTeX {arg} or [arg].
 *		1 for an arg means arg terminated by first non-alpha char
 *		2 means arg terminated by {   (e.g., as for /def)
 *		8 means arg terminated by first whitespace char
 * ======================================================================= */
/* --- entry point --- */
char	*getdirective ( char *string, char *directive,
	int iscase, int isvalid, int nargs, void *args )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
int	iarg = (-1);			/* init to signal error */
char	*pfirst = NULL,			/* ptr to 1st char of directive */
	*plast = NULL,			/* ptr past last char of last arg */
	*plbrace=NULL, *prbrace=NULL;	/* ptr to left,right brace of arg */
int	fldlen = 0;			/* #chars between { and } delims */
char	argfld[512];			/* {arg} characters */
int	nfmt=0, /*isnegfmt=0,*/		/* {arg} format */
	argfmt[9]={0,0,0,0,0,0,0,0,0};	/* argformat digits */
int	gotargs = (args==NULL?0:1);	/* true if args array supplied */
int	isdalpha = 1;			/* true if directive ends with alpha*/
char	*strpspn(char *s,char *reject,char *segment); /*non-() not in rej*/
#if 0 /* --- declared globally above --- */
/* ---
 * mathtex global variables (not used by mimetex)
 * ------------------------------------------------- */
int	noptional = 0;
char	optionalargs[10][32];
int	argformat = 0;
int	optionalpos = 0;
#endif
/* -------------------------------------------------------------------------
Find first \directive in string
-------------------------------------------------------------------------- */
noptional = 0;				/* no optional [args] yet */
for ( iarg=0; iarg<8; iarg++ )		/* for each one... */
  *optionalargs[iarg] = '\000';		/* re-init optional [arg] buffer */
if ( argformat != 0 ) {			/* have argformat */
  int	myfmt = argformat;		/* local copy */
  if ( myfmt < 0 ) { /*isnegfmt=1;*/ myfmt=(-myfmt); } /* check sign */
  while ( myfmt>0 && nfmt<9 ) {		/* have more format digits */
    argfmt[nfmt] = myfmt%10;		/* store low-order decimal digit */
    myfmt /= 10;			/* and shift it out */
    nfmt++; }				/* count another format digit */
  } /* --- end-of-if(argformat!=0) --- */
if ( isempty(directive) ) goto end_of_job; /* no input \directive given */
if ( !isalpha((int)(directive[strlen(directive)-1])) )isdalpha=0;/*not alpha*/
pfirst = string;			/* start at beginning of string */
while ( 1 ) {				/* until we find \directive */
  if ( !isempty(pfirst) )		/* still have string from caller */
    pfirst =				/* ptr to 1st char of directive */
     (iscase>0? strstr(pfirst,directive): /* case-sensistive match */
      strcasestr(pfirst,directive));	/* case-insensistive match */
  if ( isempty(pfirst) ) {		/* \directive not found in string */
    pfirst = NULL;			/* signal \directive not found */
    goto end_of_job; }			/* quit, signalling error to caller*/
  plast = pfirst + strlen(directive);	/*ptr to fist char past directive*/
  if ( !isdalpha || !isalpha((int)(*plast)) ) break; /* found \directive */
  pfirst = plast;			/* keep looking */
  plast = NULL;				/* reset plast */
  } /* --- end-of-while(1) --- */
if ( nargs < 0 ) {			/* optional [arg] may be present */
  nargs = -nargs;			/* flip sign back to positive */
  /*noptional = 1;*/ }			/* and set optional flag */
/* -------------------------------------------------------------------------
Get arguments
-------------------------------------------------------------------------- */
iarg = 0;				/* no args yet */
if ( nargs > 0 )			/* \directive has {args} */
  while ( iarg < nargs+noptional ) {	/* get each arg */
    int karg = iarg-noptional;		/* non-optional arg index */
    int kfmt = (nfmt<=karg?0:argfmt[nfmt-karg-1]); /* arg format digit */
    /* --- find left { and right } arg delimiters --- */
    plbrace = plast;			/*ptr to fist char past previous arg*/
    skipwhite(plbrace);			/* push it to first non-white char */
    if ( isempty(plbrace) ) break;	/* reached end-of-string */
    /* --- check LaTeX for single-char arg or {arg} or optional [arg] --- */
    if ( kfmt == 0 ) {			/* interpret LaTeX {arg} format */
     if ( !isthischar(*plbrace,(iarg==optionalpos+noptional?"{[":"{")) ) {
      /* --- single char argument --- */
      plast = plbrace + 1;		/* first char after single-char arg*/
      argfld[0] = *plbrace;		/* arg field is this one char */
      argfld[1] = '\000'; }		/* null-terminate field */
     else {				/* have {arg} or optional [arg] */
      /* note: to use for validation, need robust {} match like strpspn() */
      if ( (prbrace = strchr(plbrace,(*plbrace=='{'?'}':']'))) /*right }or]*/
      ==   NULL ) break;		/*and no more args if no right brace*/
      if ( 1 )				/* true to use strpspn() */
	prbrace = strpspn(plbrace,NULL,NULL); /* push to matching } or ] */
      plast = prbrace + 1;		/* first char after right brace */
      /* --- extract arg field between { and } delimiters --- */
      fldlen = (int)(prbrace-plbrace) - 1; /* #chars between { and } delims*/
      if ( fldlen >= 256 ) fldlen=255;	/* don't overflow argfld[] buffer */
      if ( fldlen > 0 )			/* have chars in field */
        memcpy(argfld,plbrace+1,fldlen); /*copy field chars to local buffer*/
      argfld[fldlen] = '\000';		/* and null-terminate field */
      trimwhite(argfld);		/* trim whitespace from argfld */
      } /* --- end-of-if/else(!isthischar(*plbrace,...)) --- */
     } /* --- end-of-if(kfmt==0) --- */
    /* --- check plain TeX for arg terminated by whitespace --- */
    if ( kfmt != 0 ) {			/* interpret plain TeX arg format */
     char *parg = NULL;			/* ptr into arg, used as per kfmt */
     plast = plbrace;			/* start at first char of arg */
     if ( *plast == '\\' ) plast++;	/* skip leading \command backslash */
     /* --- interpret arg according to its format --- */
     switch ( kfmt ) {
       case 1:
       default: skipcommand(plast); break; /* push ptr to non-alpha char */
       case 2: parg = strchr(plast,'{'); /* next arg always starts with { */
	if ( parg != NULL ) plast=parg; else plast++; /* up to { or 1 char */
	break;
       case 8: findwhite(plast); break;	/*ptr to whitespace after last char*/
       } /* --- end-of-switch(kfmt) --- */
     /* --- extract arg field --- */
     fldlen = (int)(plast-plbrace);	/* #chars between in field */
     if ( fldlen >= 256 ) fldlen=255;	/* don't overflow argfld[] buffer */
     if ( fldlen > 0 )			/* have chars in field */
       memcpy(argfld,plbrace,fldlen);	/*copy field chars to local buffer*/
     argfld[fldlen] = '\000';		/* and null-terminate field */
     if ( 1 ) { trimwhite(argfld); }	/* trim whitespace from argfld */
     } /* --- end-of-if(kfmt!=0) --- */
    if ( isvalid != 0 ) {		/* argfld[] validity check desired */
     if ( isvalid == 1 ) {		/* numeric check wanted */
       int validlen = strspn(argfld," +-.0123456789"); /*very simple check*/
       argfld[validlen] = '\000'; }	/* truncate invalid chars */
     } /* --- end-of-if(isvalid!=0) --- */
    /* --- store argument field in caller's array --- */
    if ( kfmt==0 && *plbrace=='[' ) {	/*store [arg] as optionalarg instead*/
     if ( noptional < 8 ) {		/* don't overflow our buffer */
       strninit(optionalargs[noptional],argfld,254); } /*copy to optionalarg*/
     noptional++; }			/* count another optional [arg] */
    else				/*{args} returned in caller's array*/
     if ( gotargs ) {			/*caller supplied address or array*/
      if ( nargs < 2 )			/*just one arg, so it's an address*/ 
        strcpy((char *)args,argfld);	/* so copy arg field there */
      else {				/* >1 arg, so it's a ptr array */
	char *argptr = ((char **)args)[karg]; /* arg ptr in array of ptrs */
	if ( argptr != NULL )		/* array has iarg-th address */
	  strcpy(argptr,argfld);	/* so copy arg field there */
	else gotargs = 0; } }		/* no more addresses in array */
    /* --- completed this arg --- */
    iarg++;				/* bump arg count */
    } /* --- end-of-while(iarg<nargs) --- */
/* -------------------------------------------------------------------------
Back to caller
-------------------------------------------------------------------------- */
end_of_job:
  if ( 1 ) argformat = 0;		/* always/never reset global arg */
  if ( 1 ) optionalpos = 0;		/* always/never reset global arg */
  if ( pfirst!=NULL && plast!=NULL )	/* have directive field delims */
    {strsqueeze(pfirst,((int)(plast-pfirst)));} /* squeeze out directive */
  return ( pfirst );			/* ptr to 1st char after directive */
} /* --- end-of-function getdirective() --- */


/* ==========================================================================
 * Function:	strpspn ( char *s, char *reject, char *segment )
 * Purpose:	finds the initial segment of s containing no chars
 *		in reject that are outside (), [] and {} parens, e.g.,
 *		   strpspn("abc(---)def+++","+-",segment) returns
 *		   segment="abc(---)def" and a pointer to the first '+' in s
 *		because the -'s are enclosed in () parens.
 * --------------------------------------------------------------------------
 * Arguments:	s (I)		(char *)pointer to null-terminated string
 *				whose initial segment is desired
 *		reject (I)	(char *)pointer to null-terminated string
 *				containing the "reject chars"
 *				If reject contains a " or a ', then the
 *				" or ' isn't itself a reject char,
 *				but other reject chars within quoted
 *				strings (or substrings of s) are spanned.
 *		segment (O)	(char *)pointer returning null-terminated
 *				string comprising the initial segment of s
 *				that contains non-rejected chars outside
 *				(),[],{} parens, i.e., all the chars up to
 *				but not including the returned pointer.
 *				(That's the entire string if no non-rejected
 *				chars are found.)
 * --------------------------------------------------------------------------
 * Returns:	( char * )	pointer to first reject-char found in s
 *				outside parens, or a pointer to the
 *				terminating '\000' of s if there are
 *				no reject chars in s outside all () parens.
 *				But if reject is empty, returns pointer
 *				to matching )]} outside all parens.
 * --------------------------------------------------------------------------
 * Notes:     o	the return value is _not_ like strcspn()'s
 *	      o	improperly nested (...[...)...] are not detected,
 *		but are considered "balanced" after the ]
 *	      o	if reject not found, segment returns the entire string s
 *	      o	but if reject is empty, returns segment up to and including
 *		matching )]}
 *	      o	leading/trailing whitespace is trimmed from returned segment
 * ======================================================================= */
/* --- entry point --- */
char	*strpspn ( char *s, char *reject, char *segment )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
char	*ps = s;			/* current pointer into s */
char	*strqspn(char *s,char *q,int isunescape); /*span quoted string*/
char	qreject[256]="\000", *pq=qreject, *pr=reject; /*find "or' in reject*/
int	isqspan = 0;			/* true to span quoted strings */
int	depth = 0;			/* () paren nesting level */
int	seglen=0, maxseg=2047;		/* segment length, max allowed */
int	isescaped=0, checkescapes=1;	/* signals escaped chars */
/* -------------------------------------------------------------------------
initialization
-------------------------------------------------------------------------- */
/* --- check arguments --- */
if ( isempty(s)				/* no input string supplied */
/*||   isempty(reject)*/ ) goto end_of_job; /* no reject chars supplied */
/* --- set up qreject w/o quotes --- */
if ( !isempty(reject) )			/* have reject string from caller */
  while ( *pr != '\000' ) {		/* until end-of-reject string */
    if ( !isthischar(*pr,"\"\'") )	/* not a " or ' */
      *pq++ = *pr;			/* copy actual reject char */
    else isqspan = 1;			/* span rejects in quoted strings */
    pr++; }				/* next reject char from caller */
*pq = '\000';				/* null-terminate qreject */
/* -------------------------------------------------------------------------
find first char from s outside () parens (and outside ""'s) and in reject
-------------------------------------------------------------------------- */
while ( *ps != '\000' ) {		/* search till end of input string */
  int spanlen = 1;			/*span 1 non-reject, non-quoted char*/
  if ( !isescaped ) {			/* ignore escaped \(,\[,\{,\),\],\}*/
    if ( isthischar(*ps,"([{") ) depth++;   /* push another paren */
    if ( isthischar(*ps,")]}") ) depth--; } /* or pop another paren */
  if ( depth < 1 ) {			/* we're outside all parens */
    if ( isqspan )			/* span rejects in quoted strings */
      if ( isthischar(*ps,"\"\'") ) {	/* and we're at opening quote */
        pq = strqspn(ps,NULL,0);	/* locate matching closing quote */
        if ( pq != ps )			/* detected start of quoted string */
         if ( *pq == *ps )		/* and found closing quote */
          spanlen = ((int)(pq-ps)) + 1; } /* span the entire quoted string */
    if ( isempty(qreject) ) break;	/* no reject so break immediately */
    if ( isthischar(*ps,qreject) ) break; } /* only break on a reject char */
  if ( checkescapes )			/* if checking escape sequences */
    isescaped = (*ps=='\\'?1:0);	/* reset isescaped signal */
  if ( segment != NULL ) {		/* caller gave us segment */
    int copylen = min2(spanlen,maxseg-seglen); /* don't overflow segment */
    if ( copylen > 0 )			/* have room in segment buffer */
      memcpy(segment+seglen,ps,copylen); } /* so copy non-reject chars */
  seglen += spanlen;  ps += spanlen;	/* bump to next char */
  } /* --- end-of-while(*ps!=0) --- */
end_of_job:
  if ( segment != NULL ) {		/* caller gave us segment */
    if ( isempty(qreject) && !isempty(s) ) { /* no reject char */
      segment[min2(seglen,maxseg)] = *ps;  seglen++; } /*closing )]} to seg*/
    segment[min2(seglen,maxseg)] = '\000'; /* null-terminate the segment */
    trimwhite(segment); }		/* trim leading/trailing whitespace*/
  return ( ps );			/* back to caller */
} /* --- end-of-function strpspn() --- */


/* ==========================================================================
 * Function:	strqspn ( char *s, char *q, int isunescape )
 * Purpose:	finds matching/closing " or ' in quoted string
 *		that begins with " or ', and optionally changes
 *		escaped quotes to unescaped quotes.
 * --------------------------------------------------------------------------
 * Arguments:	s (I)		(char *)pointer to null-terminated string
 *				that begins with " or ',
 *		q (O)		(char *)pointer returning null-terminated
 *				quoted token, with or without outer quotes,
 *				and with or without escaped inner quotes
 *				changed to unescaped quotes, depending
 *				on isunescape.
 *		isunescape (I)	int containing 1 to change \" to " if s
 *				is "quoted" or change \' to ' if 'quoted',
 *				or containing 2 to change both \" and \'
 *				to unescaped quotes.  Other \sequences aren't
 *				changed.  Note that \\" emits \".
 *				isunescape=0 makes no changes at all.
 *				Note: the following not implemented yet --
 *				If unescape is negative, its abs() is used,
 *				but outer quotes aren't included in q.
 * --------------------------------------------------------------------------
 * Returns:	( char * )	pointer to matching/closing " or '
 *				(or to char after quote if isunescape<0),
 *				or terminating '\000' if none found,
 *				or unchanged (same as s) if not quoted string
 * --------------------------------------------------------------------------
 * Notes:     o
 * ======================================================================= */
/* --- entry point --- */
char	*strqspn ( char *s, char *q, int isunescape )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
char	*ps = s,  *pq = q;		/* current pointer into s, q */
char	quote = '\000';			/* " or ' quote character */
int	qcopy = (isunescape<0?0:1);	/* true to copy outer quotes */
int	isescaped = 0;			/* true to signal \escape sequence */
int	maxqlen = 2400;			/* max length of returned q */
/* -------------------------------------------------------------------------
Initialization
-------------------------------------------------------------------------- */
/* --- check args --- */
if ( s == NULL ) goto end_of_job;	/* no string supplied */
skipwhite(ps);				/* skip leading whitespace */
if ( *ps == '\000'			/* string exhausted */
||   (!isthischar(*ps,"\"\'")) ) {	/* or not a " or ' quoted string */
  ps = s;  goto end_of_job; }		/*signal error/not string to caller*/
if ( isunescape < 0 ) isunescape = (-isunescape); /* flip positive */
/* --- set quote character --- */
quote = *ps;				/* set quote character */
if ( qcopy && q!=NULL ) *pq++ = quote;	/* and copy it to output token */
/* -------------------------------------------------------------------------
span characters between quotes
-------------------------------------------------------------------------- */
while ( *(++ps) != '\000' ) {		/* end-of-string always terminates */
  /* --- process escaped chars --- */
  if ( isescaped ) {			/* preceding char was \ */
    if ( *ps != '\\' ) isescaped = 0;	/* reset isescaped flag unless \\ */
    if ( q != NULL ) {			/* caller wants quoted token */
      if ( isunescape==0		/* don't unescape anything */
      ||   (isunescape==1 && *ps!=quote) /* escaped char not our quote */
      ||   (isunescape==2 && (!isthischar(*ps,"\"\'"))) ) /* not any quote */
        if ( --maxqlen > 0 )		/* so if there's room in token */
          *pq++ = '\\';			/*keep original \ in returned token*/
      if ( !isescaped )			/* will have to check 2nd \ in \\ */
        if ( --maxqlen > 0 )		/* if there's room in token */
          *pq++ = *ps; }		/* put escaped char in token */
    continue; }				/* go on to next char in string */
  /* --- check if next char escaped --- */
  if ( *ps == '\\' ) {			/* found escape char */
    isescaped=1; continue; }		/*set flag and process escaped char*/
  /* --- check for unescaped closing quote --- */
  if ( *ps == quote ) {			/* got an unescaped quote */
    if ( qcopy && q!=NULL ) *pq++ = quote; /* copy it to output token */
    if ( 0 && !qcopy ) ps++;		/* return ptr to char after quote */
    goto end_of_job; }			/* back to caller */
  /* --- process other chars --- */
  if ( q != NULL )			/* caller want token returned */
    if ( --maxqlen > 0 )		/* and there's still room in token */
      *pq++ = *ps;			/* put char in  token */
  } /* --- end-of-while(*(++ps)!='\000') --- */
/*ps = NULL;*/  /*pq = q;*/		/* error if no closing quote found */
end_of_job:
  if ( q != NULL ) *pq = '\000';	/* null-terminate returned token */
  return ( ps );			/* return ptr to " or ', or NULL */
} /* --- end-of-function strqspn() --- */


/* ==========================================================================
 * Functions:	int  unescape_url ( char *url, int isescape )
 *		char x2c ( char *what )
 * Purpose:	unescape_url replaces 3-character sequences %xx in url
 *		    with the single character represented by hex xx.
 *		x2c returns the single character represented by hex xx
 *		    passed as a 2-character sequence in what.
 * --------------------------------------------------------------------------
 * Arguments:	url (I)		char * containing null-terminated
 *				string with embedded %xx sequences
 *				to be converted.
 *		isescape (I)	int containing 1 to _not_ unescape
 *				\% sequences (0 would be NCSA default)
 *		what (I)	char * whose first 2 characters are
 *				interpreted as ascii representations
 *				of hex digits.
 * --------------------------------------------------------------------------
 * Returns:	( int )		unescape_url always returns 0.
 *		( char )	x2c returns the single char
 *				corresponding to hex xx passed in what.
 * --------------------------------------------------------------------------
 * Notes:     o	These two functions were taken verbatim from util.c in
 *   ftp://ftp.ncsa.uiuc.edu/Web/httpd/Unix/ncsa_httpd/cgi/ncsa-default.tar.Z
 *	      o	Not quite "verbatim" -- I added the "isescape logic" 4-Dec-03
 *		so unescape_url() can be safely applied to input which may or
 *		may not have been url-encoded.  (Note: currently, all calls
 *		to unescape_url() pass iescape=0, so it's not used.)
 *	      o	Added +++'s to blank xlation on 24-Sep-06
 *	      o	Added ^M,^F,etc to blank xlation 0n 01-Oct-06
 * ======================================================================= */
/* --- entry point --- */
int unescape_url(char *url, int isescape) {
    int x=0,y=0,prevescape=0,gotescape=0;
    int xlateplus = (isplusblank==1?1:0); /* true to xlate plus to blank */
    int strreplace();			/* replace + with blank, if needed */
    char x2c();
    static char *hex="0123456789ABCDEFabcdef";
    /* ---
     * xlate ctrl chars to blanks
     * -------------------------- */
    if ( 1 ) {				/* xlate ctrl chars to blanks */
      char *ctrlchars = "\n\t\v\b\r\f\a\015";
      int  seglen = strspn(url,ctrlchars); /*initial segment with ctrlchars*/
      int  urllen = strlen(url);	/* total length of url string */
      /* --- first, entirely remove ctrlchars from beginning and end --- */
      if ( seglen > 0 ) {		/*have ctrlchars at start of string*/
	strsqueeze(url,seglen);		/* squeeze out initial ctrlchars */
	urllen -= seglen; }		/* string is now shorter */
      while ( --urllen >= 0 )		/* now remove ctrlchars from end */
	if ( isthischar(url[urllen],ctrlchars) ) /* ctrlchar at end */
	  url[urllen] = '\000';		/* re-terminate string before it */
	else break;			/* or we're done */
      urllen++;				/* length of url string */
      /* --- now, replace interior ctrlchars with ~ blanks --- */
      while ( (seglen=strcspn(url,ctrlchars)) < urllen ) /*found a ctrlchar*/
	url[seglen] = '~';		/* replace ctrlchar with ~ */
      } /* --- end-of-if(1) --- */
    /* ---
     * xlate +'s to blanks if requested or if deemed necessary
     * ------------------------------------------------------- */
    if ( isplusblank == (-1) ) {	/*determine whether or not to xlate*/
      char *searchfor[] = { " ","%20", "%2B","%2b", "+++","++",
	"+=+","+-+", NULL };
      int  isearch = 0,			/* searchfor[] index */
	   nfound[11] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}; /*#occurrences*/
      /* --- locate occurrences of searchfor[] strings in url --- */
      for ( isearch=0; searchfor[isearch] != NULL; isearch++ ) {
	char *psearch = url;		/* start search at beginning */
	nfound[isearch] = 0;		/* init #occurrences count */
	while ( (psearch=strstr(psearch,searchfor[isearch])) != NULL ) {
	  nfound[isearch] += 1;		/* count another occurrence */
	  psearch += strlen(searchfor[isearch]); } /*resume search after it*/
	} /* --- end-of-for(isearch) --- */
      /* --- apply some common-sense logic --- */
      if ( nfound[0] + nfound[1] > 0 )	/* we have actual " "s or "%20"s */
	isplusblank = xlateplus = 0;	/* so +++'s aren't blanks */
      if ( nfound[2] + nfound[3] > 0 ) { /* we have "%2B" for +++'s */
        if ( isplusblank != 0 )		/* and haven't disabled xlation */
	  isplusblank = xlateplus = 1;	/* so +++'s are blanks */
	else				/* we have _both_ "%20" and "%2b" */
	  xlateplus = 0; }		/* tough call */
      if ( nfound[4] + nfound[5] > 0	/* we have multiple ++'s */
      ||   nfound[6] + nfound[7] > 0 )	/* or we have a +=+ or +-+ */
	if ( isplusblank != 0 )		/* and haven't disabled xlation */
	  xlateplus = 1;		/* so xlate +++'s to blanks */
      } /* --- end-of-if(isplusblank==-1) --- */
    if ( xlateplus > 0 ) {		/* want +'s xlated to blanks */
      char *xlateto[] = { ""," "," "," + "," "," "," "," "," " };
      while ( xlateplus > 0 ) {		/* still have +++'s to xlate */
	char plusses[99] = "++++++++++++++++++++"; /* longest +++ string */
	plusses[xlateplus] = '\000';	/* null-terminate +++'s */
	strreplace(url,plusses,xlateto[xlateplus],0); /* xlate +++'s */
	xlateplus--;			/* next shorter +++ string */
	} /* --- end-of-while(xlateplus>0) --- */
      } /* --- end-of-if(xlateplus) --- */
    isplusblank = 0;			/* don't iterate this xlation */
    /* ---
     * xlate %nn to corresponding char
     * ------------------------------- */
    for(;url[y];++x,++y) {
	gotescape = prevescape;
	prevescape = (url[x]=='\\');
	if((url[x] = url[y]) == '%')
	 if(!isescape || !gotescape)
	  if(isthischar(url[y+1],hex)
	  && isthischar(url[y+2],hex))
	    { url[x] = x2c(&url[y+1]);
	      y+=2; }
    }
    url[x] = '\0';
    return 0;
} /* --- end-of-function unescape_url() --- */
/* --- entry point --- */
char x2c(char *what) {
    char digit;
    digit = (what[0] >= 'A' ? ((what[0] & 0xdf) - 'A')+10 : (what[0] - '0'));
    digit *= 16;
    digit += (what[1] >= 'A' ? ((what[1] & 0xdf) - 'A')+10 : (what[1] - '0'));
    return(digit);
} /* --- end-of-function x2c() --- */
#endif /* PART2 */

/* ---
 * PART3
 * ------ */
#if !defined(PARTS) || defined(PART3)
/* ==========================================================================
 * Function:	rasterize ( expression, size )
 * Purpose:	returns subraster corresponding to (a valid LaTeX) expression
 *		at font size
 * --------------------------------------------------------------------------
 * Arguments:	expression (I)	char * to first char of null-terminated
 *				string containing valid LaTeX expression
 *				to be rasterized
 *		size (I)	int containing 0-4 default font size
 * --------------------------------------------------------------------------
 * Returns:	( subraster * )	ptr to subraster corresponding to expression,
 *				or NULL for any parsing error.
 * --------------------------------------------------------------------------
 * Notes:     o	This is mimeTeX's "main" reusable entry point.  Easy to use:
 *		just call it with a LaTeX expression, and get back a bitmap
 *		of that expression.  Then do what you want with the bitmap.
 * ======================================================================= */
/* --- entry point --- */
subraster *rasterize ( char *expression, int size )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
char	*preamble(), pretext[512];	/* process preamble, if present */
char	chartoken[MAXSUBXSZ+1], *texsubexpr(), /*get subexpression from expr*/
	*subexpr = chartoken;		/* token may be parenthesized expr */
int	isbrace();			/* check subexpr for braces */
mathchardef *symdef, *get_symdef();	/*get mathchardef struct for symbol*/
int	ligdef, get_ligature();		/*get symtable[] index for ligature*/
int	natoms=0;			/* #atoms/tokens processed so far */
int	type_raster();			/* display debugging output */
subraster *rasterize(),			/* recurse */
	*rastparen(),			/* handle parenthesized subexpr's */
	*rastlimits();			/* handle sub/superscripted expr's */
subraster *rastcat(),			/* concatanate atom subrasters */
	*subrastcpy(),			/* copy final result if a charaster*/
	*new_subraster();		/* new subraster for isstring mode */
subraster *get_charsubraster(),		/* character subraster */
	*sp=NULL, *prevsp=NULL,		/* raster for current, prev char */
	*expraster = (subraster *)NULL;	/* raster returned to caller */
int	delete_subraster();		/* free everything before returning*/
int	family = fontinfo[fontnum].family; /* current font family */
int	isleftscript = 0,		/* true if left-hand term scripted */
	wasscripted = 0,		/* true if preceding token scripted*/
	wasdelimscript = 0;		/* true if preceding delim scripted*/
/*int	pixsz = 1;*/			/*default #bits per pixel, 1=bitmap*/
char	*strdetex();			/* detex token for error message */
/* --- global values saved/restored at each recursive iteration --- */
int	wasstring = isstring,		/* initial isstring mode flag */
	wasdisplaystyle = isdisplaystyle, /*initial displaystyle mode flag*/
	oldfontnum = fontnum,		/* initial font family */
	oldfontsize = fontsize,		/* initial fontsize */
	olddisplaysize = displaysize,	/* initial \displaystyle size */
	oldshrinkfactor = shrinkfactor,	/* initial shrinkfactor */
	oldsmashmargin = smashmargin,	/* initial smashmargin */
	oldissmashdelta = issmashdelta, /* initial issmashdelta */
	oldisexplicitsmash = isexplicitsmash, /* initial isexplicitsmash */
	oldisscripted = isscripted,	/* initial isscripted */
	*oldworkingparam = workingparam; /* initial working parameter */
subraster *oldworkingbox = workingbox,	/* initial working box */
	*oldleftexpression = leftexpression; /*left half rasterized so far*/
double	oldunitlength = unitlength;	/* initial unitlength */
mathchardef *oldleftsymdef = leftsymdef; /* init oldleftsymdef */
/* -------------------------------------------------------------------------
initialization
-------------------------------------------------------------------------- */
recurlevel++;				/* wind up one more recursion level*/
leftexpression = NULL;			/* no leading left half yet */
isreplaceleft = 0;			/* reset replaceleft flag */
if(1)fraccenterline = NOVALUE;		/* reset \frac baseline signal */
/* shrinkfactor = shrinkfactors[max2(0,min2(size,LARGESTSIZE))];*/ /*set sf*/
shrinkfactor = shrinkfactors[max2(0,min2(size,16))]; /* have 17 sf's */
rastlift = 0;				/* reset global rastraise() lift */
if ( msgfp!=NULL && msglevel >= 9 ) {	/*display expression for debugging*/
 fprintf(msgfp,
 "rasterize> recursion#%d, size=%d,\n\texpression=\"%s\"\n",
 recurlevel,size,(expression==NULL?"<null>":expression)); fflush(msgfp); }
if ( expression == NULL ) goto end_of_job; /* nothing given to do */
/* -------------------------------------------------------------------------
preocess optional $-terminated preamble preceding expression
-------------------------------------------------------------------------- */
expression = preamble(expression,&size,pretext); /* size may be modified */
if ( *expression == '\000' ) goto end_of_job; /* nothing left to do */
fontsize = size;			/* start at requested size */
if ( isdisplaystyle == 1 )		/* displaystyle enabled but not set*/
 if ( !ispreambledollars )		/* style fixed by $$...$$'s */
  isdisplaystyle = (fontsize>=displaysize? 2:1); /*force at large fontsize*/
/* -------------------------------------------------------------------------
build up raster one character (or subexpression) at a time
-------------------------------------------------------------------------- */
while ( 1 )
  {
  /* --- kludge for \= cyrillic ligature --- */
  isligature = 0;			/* no ligature found yet */
  family = fontinfo[fontnum].family;	/* current font family */
  if ( family == CYR10 )		/* may have cyrillic \= ligature */
   if ( (ligdef = get_ligature(expression,family)) /*check for any ligature*/
   >=    0 )				/* got some ligature */
    if ( memcmp(symtable[ligdef].symbol,"\\=",2) == 0 ) /* starts with \= */
     isligature = 1;			/* signal \= ligature */
  /* --- get next character/token or subexpression --- */
  subexprptr = expression;		/* ptr within expression to subexpr*/
  expression = texsubexpr(expression,chartoken,0,LEFTBRACES,RIGHTBRACES,1,1);
  subexpr = chartoken;			/* "local" copy of chartoken ptr */
  leftsymdef = NULL;			/* no character identified yet */
  sp = NULL;				/* no subraster yet */
  size = fontsize;			/* in case reset by \tiny, etc */
  /*isleftscript = isdelimscript;*/	/*was preceding term scripted delim*/
  wasscripted = isscripted;		/* true if preceding token scripted*/
  wasdelimscript = isdelimscript;	/* preceding \right delim scripted */
  if(1)isscripted = 0;			/* no subscripted expression yet */
  isdelimscript = 0;			/* reset \right delim scripted flag*/
  /* --- debugging output --- */
  if ( msgfp!=NULL && msglevel >= 9 ) {	/* display chartoken for debugging */
   fprintf(msgfp,
   "rasterize> recursion#%d,atom#%d=\"%s\" (isligature=%d,isleftscript=%d)\n",
   recurlevel,natoms+1,chartoken,isligature,isleftscript); fflush(msgfp); }
  if ( expression == NULL		/* no more tokens */
  &&   *subexpr == '\000' ) break;	/* and this token empty */
  if ( *subexpr == '\000' ) break;	/* enough if just this token empty */
  /* --- check for parenthesized subexpression --- */
  if ( isbrace(subexpr,LEFTBRACES,1) )	/* got parenthesized subexpression */
    { if ( (sp=rastparen(&subexpr,size,prevsp)) /* rasterize subexpression */
      ==   NULL )  continue; }		/* flush it if failed to rasterize */
  else /* --- single-character atomic token --- */
   if ( !isthischar(*subexpr,SCRIPTS) )	/* scripts handled below */
    {
    /* --- first check for opening $ in \text{ if $n-m$ even} --- */
    if ( istextmode			/* we're in \text mode */
    &&   *subexpr=='$' && subexpr[1]=='\000' ) { /* and have an opening $ */
     char *endptr=NULL, mathexpr[MAXSUBXSZ+1]; /* $expression$ in \text{ }*/
     int  exprlen = 0;			/* length of $expression$ */
     int  textfontnum = fontnum;	/* current text font number */
     /*if ( (endptr=strrchr(expression,'$')) != NULL )*/ /*ptr to closing $*/
     if ( (endptr=strchr(expression,'$')) != NULL ) /* ptr to closing $ */
       exprlen = (int)(endptr-expression); /* #chars preceding closing $ */
     else {				/* no closing $ found */
       exprlen = strlen(expression);	/* just assume entire expression */
       endptr = expression + (exprlen-1); } /*and push expression to '\000'*/
     exprlen = min2(exprlen,MAXSUBXSZ);	/* don't overflow mathexpr[] */
     if ( exprlen > 0 ) {		/* have something between $$ */
       memcpy(mathexpr,expression,exprlen); /*local copy of math expression*/
       mathexpr[exprlen] = '\000';	/* null-terminate it */
       fontnum = 0;			/* set math mode */
       sp = rasterize(mathexpr,size);	/* and rasterize $expression$ */
       fontnum = textfontnum; }		/* set back to text mode */
     expression = endptr+1;		/* push expression past closing $ */
     } /* --- end-of-if(istextmode&&*subexpr=='$') --- */
    else
     /* --- otherwise, look up mathchardef for atomic token in table --- */
     if ( (leftsymdef=symdef=get_symdef(chartoken)) /*mathchardef for token*/
     ==  NULL )				/* lookup failed */
      { char literal[512] = "[?]";	/*display for unrecognized literal*/
        int  oldfontnum = fontnum;	/* error display in default mode */
        if ( msgfp!=NULL && msglevel >= 29 ) /* display unrecognized symbol*/
	 { fprintf(msgfp,"rasterize> get_symdef() failed for \"%s\"\n",
	   chartoken); fflush(msgfp); }
        sp = (subraster *)NULL;		/* init to signal failure */
        if ( warninglevel < 1 ) continue; /* warnings not wanted */
        fontnum = 0;			/* reset from \mathbb, etc */
        if ( isthischar(*chartoken,ESCAPE) ) /* we got unrecognized \escape*/
	 { /* --- so display literal {\rm~[\backslash~chartoken?]} ---  */
	   strcpy(literal,"{\\rm~[");	/* init error message token */
	   strcat(literal,strdetex(chartoken,0)); /* detex the token */
	   strcat(literal,"?]}"); }	/* add closing ? and brace */
        sp = rasterize(literal,size-1);	/* rasterize literal token */
        fontnum = oldfontnum;		/* reset font family */
        if ( sp == (subraster *)NULL ) continue; }/*flush if rasterize fails*/
     else /* --- check if we have special handler to process this token --- */
      if ( symdef->handler != NULL )	/* have a handler for this token */
       { int arg1=symdef->charnum, arg2=symdef->family, arg3=symdef->class;
         if ( (sp = (subraster *)	/* returned void* is subraster* */
	 (*(symdef->handler))(&expression,size,prevsp,arg1,arg2,arg3))==NULL)
	   continue; }			/* flush token if handler failed */
      else /* --- no handler, so just get subraster for this character --- */
       if ( !isstring )			/* rasterizing */
	{ if ( isligature )		/* found a ligature */
	   expression = subexprptr + strlen(symdef->symbol); /*push past it*/
	  if ( (sp=get_charsubraster(symdef,size)) /* get subraster */
	  ==  NULL )  continue; }	/* flush token if failed */
       else				/* constructing ascii string */
	{ char *symbol = symdef->symbol; /* symbol for ascii string */
	  int symlen = (symbol!=NULL?strlen(symbol):0); /*#chars in symbol*/
	  if ( symlen < 1 ) continue;	/* no symbol for ascii string */
	  if ( (sp=new_subraster(symlen+1,1,8)) /* subraster for symbol */
	  ==  NULL )  continue;		/* flush token if malloc failed */
	  sp->type = ASCIISTRING;	/* set subraster type */
	  sp->symdef = symdef;		/* and set symbol definition */
	  sp->baseline = 1;		/* default (should be unused) */
	  strcpy((char *)((sp->image)->pixmap),symbol); /* copy symbol */
	  /*((char *)((sp->image)->pixmap))[symlen] = '\000';*/ } /*null*/
    } /* --- end-of-if(!isthischar(*subexpr,SCRIPTS)) --- */
  /* --- handle any super/subscripts following symbol or subexpression --- */
  sp = rastlimits(&expression,size,sp);
  isleftscript = (wasscripted||wasdelimscript?1:0);/*preceding term scripted*/
  /* --- debugging output --- */
  if ( msgfp!=NULL && msglevel >= 9 ) {	/* display raster for debugging */
   fprintf(msgfp,"rasterize> recursion#%d,atom#%d%s\n",
   recurlevel,natoms+1,(sp==NULL?" = <null>":"..."));
   if ( msglevel >= 9 ) fprintf(msgfp,
    "  isleftscript=%d is/wasscripted=%d,%d is/wasdelimscript=%d,%d\n",
    isleftscript,isscripted,wasscripted,isdelimscript,wasdelimscript);
   if ( msglevel >= 99 )
    if(sp!=NULL) type_raster(sp->image,msgfp); /* display raster */
   fflush(msgfp); }			/* flush msgfp buffer */
  /* --- accumulate atom or parenthesized subexpression --- */
  if ( natoms < 1			/* nothing previous to concat */
  ||   expraster == NULL		/* or previous was complete error */
  ||   isreplaceleft )			/* or we're replacing previous */
    { if ( 1 && expraster!=NULL )	/* probably replacing left */
	delete_subraster(expraster);	/* so first free original left */
      expraster = subrastcpy(sp);	/* copy static CHARASTER or left */
      isreplaceleft = 0; }		/* reset replacement flag */
  else					/*we've already built up atoms so...*/
   if ( sp != NULL ) {			/* ...if we have a new term */
    int prevsmashmargin = smashmargin;	/* save current smash margin */
    if ( isleftscript ) {		/* don't smash against scripts */
     isdelimscript = 0;			/* reset \right delim scripted flag*/
     if ( !isexplicitsmash ) smashmargin = 0; } /* signal no smash wanted */
    expraster = rastcat(expraster,sp,1); /* concat new term, free previous */
    smashmargin = prevsmashmargin; }	/* restore current smash margin */
  delete_subraster(prevsp);		/* free prev (if not a CHARASTER) */
  prevsp = sp;				/* current becomes previous */
  leftexpression = expraster;		/* left half rasterized so far */
  /* --- bump count --- */
  natoms++;				/* bump #atoms count */
  } /* --- end-of-while(expression!=NULL) --- */
/* -------------------------------------------------------------------------
back to caller with rasterized expression
-------------------------------------------------------------------------- */
end_of_job:
  delete_subraster(prevsp);		/* free last (if not a CHARASTER) */
  /* --- debugging output --- */
  if ( msgfp!=NULL && msglevel >= 999 )	/* display raster for debugging */
    { fprintf(msgfp,"rasterize> Final recursion level=%d, atom#%d...\n",
      recurlevel,natoms);
      if ( expraster != (subraster *)NULL ) /* i.e., if natoms>0 */
	type_raster(expraster->image,msgfp); /* display completed raster */
      fflush(msgfp); }			/* flush msgfp buffer */
  /* --- set final raster buffer --- */
  if ( 1 && expraster != (subraster *)NULL ) /* have an expression */
    { int type = expraster->type;	/* type of constructed image */
      if ( type != FRACRASTER )		/* leave \frac alone */
	expraster->type = IMAGERASTER;	/* set type to constructed image */
      if ( istextmode )			/* but in text mode */
        expraster->type = blanksignal;	/* set type to avoid smash */
      expraster->size = fontsize; }	/* set original input font size */
  /* --- restore flags/values to original saved values --- */
  isstring = wasstring;			/* string mode reset */
  isdisplaystyle = wasdisplaystyle;	/* displaystyle mode reset */
  fontnum = oldfontnum;			/* font family reset */
  fontsize = oldfontsize;		/* fontsize reset */
  displaysize = olddisplaysize;		/* \displaystyle size reset */
  shrinkfactor = oldshrinkfactor;	/* shrinkfactor reset */
  smashmargin = oldsmashmargin;		/* smashmargin reset */
  issmashdelta = oldissmashdelta;	/* issmashdelta reset */
  isexplicitsmash = oldisexplicitsmash;	/* isexplicitsmash reset */
  isscripted = oldisscripted;		/* isscripted reset */
  workingparam = oldworkingparam;	/* working parameter reset */
  workingbox = oldworkingbox;		/* working box reset */
  leftexpression = oldleftexpression;	/* leftexpression reset */
  leftsymdef = oldleftsymdef;		/* leftsymdef reset */
  unitlength = oldunitlength;		/* unitlength reset */
  iunitlength = (int)(unitlength+0.5);	/* iunitlength reset */
  recurlevel--;				/* unwind one recursion level */
  /* --- return final subraster to caller --- */
  return ( expraster );
} /* --- end-of-function rasterize() --- */


/* ==========================================================================
 * Function:	rastparen ( subexpr, size, basesp )
 * Purpose:	parentheses handler, returns a subraster corresponding to
 *		parenthesized subexpression at font size
 * --------------------------------------------------------------------------
 * Arguments:	subexpr (I)	char **  to first char of null-terminated
 *				string beginning with a LEFTBRACES
 *				to be rasterized
 *		size (I)	int containing 0-7 default font size
 *		basesp (I)	subraster *  to character (or subexpression)
 *				immediately preceding leading left{
 *				(unused, but passed for consistency)
 * --------------------------------------------------------------------------
 * Returns:	( subraster * )	ptr to subraster corresponding to subexpr,
 *				or NULL for any parsing error
 * --------------------------------------------------------------------------
 * Notes:     o	This "handler" isn't in the mathchardef symbol table,
 *		but is called directly from rasterize(), as necessary.
 *	      o	Though subexpr is returned unchanged, it's passed as char **
 *		for consistency with other handlers.  Ditto, basesp is unused
 *		but passed for consistency
 * ======================================================================= */
/* --- entry point --- */
subraster *rastparen ( char **subexpr, int size, subraster *basesp )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
char	*expression = *subexpr;		/* dereference subexpr to get char* */
int	explen = strlen(expression);	/* total #chars, including parens */
int	isescape = 0,			/* true if parens \escaped */
	isrightdot = 0,			/* true if right paren is \right. */
	isleftdot = 0;			/* true if left paren is \left. */
char	left[32], right[32];		/* parens enclosing expresion */
char	noparens[MAXSUBXSZ+1];		/* get subexpr without parens */
subraster *rasterize(), *sp=NULL;	/* rasterize what's between ()'s */
int	isheight = 1;			/*true=full height, false=baseline*/
int	height,				/* height of rasterized noparens[] */
	baseline;			/* and its baseline */
int	family = /*CMSYEX*/ CMEX10;	/* family for paren chars */
subraster *get_delim(), *lp=NULL, *rp=NULL; /* left and right paren chars */
subraster *rastcat();			/* concatanate subrasters */
int	delete_subraster();		/*in case of error after allocation*/
/* -------------------------------------------------------------------------
rasterize "interior" of expression, i.e., without enclosing parens
-------------------------------------------------------------------------- */
/* --- first see if enclosing parens are \escaped --- */
if ( isthischar(*expression,ESCAPE) )	/* expression begins with \escape */
  isescape = 1;				/* so set flag accordingly */
/* --- get expression *without* enclosing parens --- */
strcpy(noparens,expression);		/* get local copy of expression */
noparens[explen-(1+isescape)] = '\000';	/* null-terminate before right} */
strsqueeze(noparens,(1+isescape));	/* and then squeeze out left{ */
/* --- rasterize it --- */
if ( (sp = rasterize(noparens,size))	/*rasterize "interior" of expression*/
==   NULL ) goto end_of_job;		/* quit if failed */
/* --- no need to add parentheses for unescaped { --- */
if ( !isescape && isthischar(*expression,"{") ) /* don't add parentheses */
  goto end_of_job;			/* just return sp to caller */
/* -------------------------------------------------------------------------
obtain paren characters to enclose noparens[] raster with
-------------------------------------------------------------------------- */
/* --- first get left and right parens from expression --- */
memset(left,0,16);  memset(right,0,16);	/* init parens with nulls */
left[0] = *(expression+isescape);	/* left{ is 1st or 2nd char */
right[0] = *(expression+explen-1);	/* right} is always last char */
isleftdot  = (isescape && isthischar(*left,".")); /* true if \left. */
isrightdot = (isescape && isthischar(*right,".")); /* true if \right. */
/* --- need height of noparens[] raster as minimum parens height --- */
height = (sp->image)->height;		/* height of noparens[] raster */
baseline = sp->baseline;		/* baseline of noparens[] raster */
if ( !isheight ) height = baseline+1;	/* parens only enclose baseline up */
/* --- get best-fit parentheses characters --- */
if ( !isleftdot )			/* if not \left. */
  lp = get_delim(left,height+1,family);	/* get left paren char */
if ( !isrightdot )			/* and if not \right. */
  rp = get_delim(right,height+1,family); /* get right paren char */
if ( (lp==NULL && !isleftdot)		/* check that we got left( */
||   (rp==NULL && !isrightdot) )	/* and right) if needed */
  { delete_subraster(sp);		/* if failed, free subraster */
    if ( lp != NULL ) free ((void *)lp);/*free left-paren subraster envelope*/
    if ( rp != NULL ) free ((void *)rp);/*and right-paren subraster envelope*/
    sp = (subraster *)NULL;		/* signal error to caller */
    goto end_of_job; }			/* and quit */
/* -------------------------------------------------------------------------
set paren baselines to center on noparens[] raster, and concat components
-------------------------------------------------------------------------- */
/* --- set baselines to center paren chars on raster --- */
if ( lp != NULL )			/* ignore for \left. */
  lp->baseline = baseline + ((lp->image)->height - height)/2;
if ( rp != NULL )			/* ignore for \right. */
  rp->baseline = baseline + ((rp->image)->height - height)/2;
/* --- concat lp||sp||rp to obtain final result --- */
if ( lp != NULL )			/* ignore \left. */
  sp = rastcat(lp,sp,3);		/* concat lp||sp and free sp,lp */
if ( sp != NULL )			/* succeeded or ignored \left. */
  if ( rp != NULL )			/* ignore \right. */
    sp = rastcat(sp,rp,3);		/* concat sp||rp and free sp,rp */
/* --- back to caller --- */
end_of_job:
  return ( sp );
} /* --- end-of-function rastparen() --- */


/* ==========================================================================
 * Function:	rastlimits ( expression, size, basesp )
 * Purpose:	\limits, \nolimts, _ and ^ handler,
 *		dispatches call to rastscripts() or to rastdispmath()
 *		as necessary, to handle sub/superscripts following symbol
 * --------------------------------------------------------------------------
 * Arguments:	expression (I)	char **  to first char of null-terminated
 *				LaTeX expression (unused/unchanged)
 *		size (I)	int containing base font size (not used,
 *				just stored in subraster)
 *		basesp (I)	subraster *  to current character (or
 *				subexpression) immediately preceding script
 *				indicator
 * --------------------------------------------------------------------------
 * Returns:	( subraster * )	ptr to subraster returned by rastscripts()
 *				or rastdispmath(), or NULL for any error
 * --------------------------------------------------------------------------
 * Notes:     o
 * ======================================================================= */
/* --- entry point --- */
subraster *rastlimits ( char **expression, int size, subraster *basesp )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
subraster *rastscripts(), *rastdispmath(), /*one of these will do the work*/
	*rastcat(),			/* may need to concat scripts */
	*rasterize(),			/* may need to construct dummy base*/
	*scriptsp = basesp,		/* and this will become the result */
	*dummybase = basesp;		/* for {}_i construct a dummy base */
int	isdisplay = (-1);		/* set 1 for displaystyle, else 0 */
int	oldsmashmargin = smashmargin;	/* save original smashmargin */
int	type_raster();			/* display debugging output */
int	delete_subraster();		/* free dummybase, if necessary */
int	rastsmashcheck();		/* check if okay to smash scripts */
/* --- to check for \limits or \nolimits preceding scripts --- */
char	*texchar(), *exprptr=*expression, limtoken[255]; /*check for \limits*/
int	toklen=0;			/* strlen(limtoken) */
mathchardef *tokdef, *get_symdef();	/* mathchardef struct for limtoken */
int	class=(leftsymdef==NULL?NOVALUE:leftsymdef->class); /*base sym class*/
/* -------------------------------------------------------------------------
determine whether or not to use displaymath
-------------------------------------------------------------------------- */
scriptlevel++;				/* first, increment subscript level*/
*limtoken = '\000';			/* no token yet */
isscripted = 0;				/* signal term not (text) scripted */
if ( msgfp!=NULL && msglevel>=999 )
 { fprintf(msgfp,"rastlimits> scriptlevel#%d exprptr=%.48s\n",
   scriptlevel,(exprptr==NULL?"null":exprptr));  fflush(msgfp); }
if ( isstring ) goto end_of_job;	/* no scripts for ascii string */
/* --- check for \limits or \nolimits --- */
skipwhite(exprptr);			/* skip white space before \limits */
if ( !isempty(exprptr) )		/* non-empty expression supplied */
  exprptr = texchar(exprptr,limtoken);	/* retrieve next token */
if ( *limtoken != '\000' )		/* have token */
 if ( (toklen=strlen(limtoken)) >= 3 )	/* which may be \[no]limits */
  if ( memcmp("\\limits",limtoken,toklen) == 0     /* may be \limits */
  ||   memcmp("\\nolimits",limtoken,toklen) == 0 ) /* or may be \nolimits */
   if ( (tokdef= get_symdef(limtoken))	/* look up token to be sure */
   !=   NULL ) {			/* found token in table */
    if ( strcmp("\\limits",tokdef->symbol) == 0 )  /* found \limits */
      isdisplay = 1;			/* so explicitly set displaymath */
    else				/* wasn't \limits */
      if ( strcmp("\\nolimits",tokdef->symbol) == 0 ) /* found \nolimits */
	isdisplay = 0; }		/* so explicitly reset displaymath */
/* --- see if we found \[no]limits --- */
if ( isdisplay != (-1) )		/* explicit directive found */
  *expression = exprptr;		/* so bump expression past it */
else					/* noexplicit directive */
  { isdisplay = 0;			/* init displaymath flag off */
    if ( isdisplaystyle ) {		/* we're in displaystyle math mode */
      if ( isdisplaystyle >= 5 )	/* and mode irrevocably forced true */
	{ if ( class!=OPENING && class!=CLOSING ) /*don't force ('s and )'s*/
	    isdisplay = 1; }		/* set flag if mode forced true */
      else
       if ( isdisplaystyle >= 2 )	/*or mode forced conditionally true*/
	{ if ( class!=VARIABLE && class!=ORDINARY /*don't force characters*/
	  &&   class!=OPENING  && class!=CLOSING  /*don't force ('s and )'s*/
	  &&   class!=BINARYOP		/* don't force binary operators */
	  &&   class!=NOVALUE )		/* finally, don't force "images" */
	    isdisplay = 1; }		/* set flag if mode forced true */
       else				/* determine mode from base symbol */
	if ( class == DISPOPER )	/* it's a displaystyle operator */
	  isdisplay = 1; } }		/* so set flag */
/* -------------------------------------------------------------------------
dispatch call to create sub/superscripts
-------------------------------------------------------------------------- */
if ( isdisplay )			/* scripts above/below base symbol */
  scriptsp = rastdispmath(expression,size,basesp); /* everything all done */
else {					/* scripts alongside base symbol */
  if ( dummybase == NULL )		/* no base symbol preceding scripts*/
    dummybase = rasterize("\\rule0{10}",size); /*guess a typical base symbol*/
  issmashokay = 1;			/*haven't found a no-smash char yet*/
  if((scriptsp=rastscripts(expression,size,dummybase)) == NULL) /*no scripts*/
    scriptsp = basesp;			/* so just return unscripted symbol*/
  else {				/* symbols followed by scripts */
    isscripted = 1;			/*signal current term text-scripted*/
    if ( basesp != NULL )		/* have base symbol */
     { /*if(0)smashmargin = 0;*/	/*don't smash script (doesn't work)*/
       /*scriptsp = rastcat(basesp,scriptsp,2);*//*concat scripts to base sym*/
       /* --- smash (or just concat) script raster against base symbol --- */
       if ( !issmashokay )		/* don't smash leading - */
         if ( !isexplicitsmash ) scriptsp->type = blanksignal; /*don't smash*/
       scriptsp = rastcat(basesp,scriptsp,3); /*concat scripts to base sym*/
       if(1) scriptsp->type = IMAGERASTER; /* flip type of composite object */
       /* --- smash (or just concat) scripted term to stuff to its left --- */
       issmashokay = 1;			/* okay to smash base expression */
       if ( 0 && smashcheck > 1 )	/* smashcheck=2 to check base */
         /* note -- we _don't_ have base expression available to check */
         issmashokay = rastsmashcheck(*expression); /*check if okay to smash*/
       if ( !issmashokay )		/* don't smash leading - */
         if ( !isexplicitsmash ) scriptsp->type = blanksignal; /*don't smash*/
       scriptsp->size = size; } } }	/* and set font size */
end_of_job:
  smashmargin = oldsmashmargin;		/* reset original smashmargin */
  if ( dummybase != basesp ) delete_subraster(dummybase); /*free work area*/
  if ( msgfp!=NULL && msglevel>=99 )
    { fprintf(msgfp,"rastlimits> scriptlevel#%d returning %s\n",
	scriptlevel,(scriptsp==NULL?"null":"..."));
      if ( scriptsp != NULL )		/* have a constructed raster */
	type_raster(scriptsp->image,msgfp); /*display constructed raster*/
      fflush(msgfp); }
  scriptlevel--;			/*lastly, decrement subscript level*/
  return ( scriptsp );
} /* --- end-of-function rastlimits() --- */


/* ==========================================================================
 * Function:	rastscripts ( expression, size, basesp )
 * Purpose:	super/subscript handler, returns subraster for the leading
 *		scripts in expression, whose base symbol is at font size
 * --------------------------------------------------------------------------
 * Arguments:	expression (I/O) char **  to first char of null-terminated
 *				string beginning with a super/subscript,
 *				and returning ptr immediately following
 *				last script character processed.
 *		size (I)	int containing 0-7 default font size
 *		basesp (I)	subraster *  to character (or subexpression)
 *				immediately preceding leading script
 *				(scripts will be placed relative to base)
 * --------------------------------------------------------------------------
 * Returns:	( subraster * )	ptr to subraster corresponding to scripts,
 *				or NULL for any parsing error
 * --------------------------------------------------------------------------
 * Notes:     o	This "handler" isn't in the mathchardef symbol table,
 *		but is called directly from rasterize(), as necessary.
 * ======================================================================= */
/* --- entry point --- */
subraster *rastscripts ( char **expression, int size, subraster *basesp )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
char	*texscripts(),			/* parse expression for scripts */
	subscript[512], supscript[512];	/* scripts parsed from expression */
subraster *rasterize(), *subsp=NULL, *supsp=NULL; /* rasterize scripts */
subraster *new_subraster(), *sp=NULL,	/* super- over subscript subraster */
	*rastack();			/*sets scripts in displaymath mode*/
raster	*rp=NULL;			/* image raster embedded in sp */
int	height=0, width=0,  baseline=0,	/* height,width,baseline of sp */
	subht=0,  subwidth=0,  subln=0,	/* height,width,baseline of sub */
	supht=0,  supwidth=0,  supln=0,	/* height,width,baseline of sup */
	baseht=0, baseln=0;		/* height,baseline of base */
int	bdescend=0, sdescend=0;		/* descender of base, subscript */
int	issub=0, issup=0, isboth=0,	/* true if we have sub,sup,both */
	isbase=0;			/* true if we have base symbol */
int	szval = min2(max2(size,0),LARGESTSIZE), /* 0...LARGESTSIZE */
	vbetween = 2,			/* vertical space between scripts */
	vabove   = szval+1,		/*sup's top/bot above base's top/bot*/
	vbelow   = szval+1,		/*sub's top/bot below base's top/bot*/
	vbottom  = szval+1;		/*sup's bot above (sub's below) bsln*/
/*int	istweak = 1;*/			/* true to tweak script positioning */
int	rastput();			/*put scripts in constructed raster*/
int	delete_subraster();		/* free work areas */
int	rastsmashcheck();		/* check if okay to smash scripts */
int	pixsz = 1;			/*default #bits per pixel, 1=bitmap*/
/* -------------------------------------------------------------------------
Obtain subscript and/or superscript expressions, and rasterize them/it
-------------------------------------------------------------------------- */
/* --- parse for sub,superscript(s), and bump expression past it(them) --- */
if ( expression == NULL ) goto end_of_job; /* no *ptr given */
if ( *expression == NULL ) goto end_of_job; /* no expression given */
if ( *(*expression) == '\000' ) goto end_of_job; /* nothing in expression */
*expression = texscripts(*expression,subscript,supscript,3);
/* --- rasterize scripts --- */
if ( *subscript != '\000' )		/* have a subscript */
  subsp = rasterize(subscript,size-1);	/* so rasterize it at size-1 */
if ( *supscript != '\000' )		/* have a superscript */
  supsp = rasterize(supscript,size-1);	/* so rasterize it at size-1 */
/* --- set flags for convenience --- */
issub  = (subsp != (subraster *)NULL);	/* true if we have subscript */
issup  = (supsp != (subraster *)NULL);	/* true if we have superscript */
isboth = (issub && issup);		/* true if we have both */
if (!issub && !issup) goto end_of_job;	/* quit if we have neither */
/* --- check for leading no-smash chars (if enabled) --- */
issmashokay = 0;			/* default, don't smash scripts */
if ( smashcheck > 0 ) {			/* smash checking wanted */
 issmashokay = 1;			/*haven't found a no-smash char yet*/
 if ( issub )				/* got a subscript */
  issmashokay = rastsmashcheck(subscript); /* check if okay to smash */
 if ( issmashokay )			/* clean sub, so check sup */
  if ( issup )				/* got a superscript */
   issmashokay = rastsmashcheck(supscript); /* check if okay to smash */
 } /* --- end-of-if(smashcheck>0) --- */
/* -------------------------------------------------------------------------
get height, width, baseline of scripts,  and height, baseline of base symbol
-------------------------------------------------------------------------- */
/* --- get height and width of components --- */
if ( issub )				/* we have a subscript */
  { subht    = (subsp->image)->height;	/* so get its height */
    subwidth = (subsp->image)->width;	/* and width */
    subln    =  subsp->baseline; }	/* and baseline */
if ( issup )				/* we have a superscript */
  { supht    = (supsp->image)->height;	/* so get its height */
    supwidth = (supsp->image)->width;	/* and width */
    supln    =  supsp->baseline; }	/* and baseline */
/* --- get height and baseline of base, and descender of base and sub --- */
if ( basesp == (subraster *)NULL )	/* no base symbol for scripts */
  basesp = leftexpression;		/* try using left side thus far */
if ( basesp != (subraster *)NULL )	/* we have base symbol for scripts */
  { baseht   = (basesp->image)->height;	/* height of base symbol */
    baseln   =  basesp->baseline;	/* and its baseline */
    bdescend =  baseht-(baseln+1);	/* and base symbol descender */
    sdescend =  bdescend + vbelow;	/*sub must descend by at least this*/
    if ( baseht > 0 ) isbase = 1; }	/* set flag */
/* -------------------------------------------------------------------------
determine width of constructed raster
-------------------------------------------------------------------------- */
width = max2(subwidth,supwidth);	/*widest component is overall width*/
/* -------------------------------------------------------------------------
determine height and baseline of constructed raster
-------------------------------------------------------------------------- */
/* --- both super/subscript --- */
if ( isboth )				/*we have subscript and superscript*/
  { height = max2(subht+vbetween+supht,	/* script heights + space bewteen */
		vbelow+baseht+vabove);	/*sub below base bot, sup above top*/
    baseline = baseln + (height-baseht)/2; } /*center scripts on base symbol*/
/* --- superscript only --- */
if ( !issub )				/* we only have a superscript */
  { height = max3(baseln+1+vabove,	/* sup's top above base symbol top */
		supht+vbottom,		/* sup's bot above baseln */
		supht+vabove-bdescend);	/* sup's bot above base symbol bot */
    baseline = height-1; }		/*sup's baseline at bottom of raster*/
/* --- subscript only --- */
if ( !issup ) {				/* we only have a subscript */
  if ( subht > sdescend )		/*sub can descend below base bot...*/
    { height = subht;			/* ...without extra space on top */
      baseline = height-(sdescend+1);	/* sub's bot below base symbol bot */
      baseline = min2(baseline,max2(baseln-vbelow,0)); }/*top below base top*/
  else					/* sub's top will be below baseln */
    { height = sdescend+1;		/* sub's bot below base symbol bot */
      baseline = 0; } }			/* sub's baseline at top of raster */
/* -------------------------------------------------------------------------
construct raster with superscript over subscript
-------------------------------------------------------------------------- */
/* --- allocate subraster containing constructed raster --- */
if ( (sp=new_subraster(width,height,pixsz)) /*allocate subraster and raster*/
==   NULL )				/* and if we fail to allocate */
  goto end_of_job;			/* quit */
/* --- initialize subraster parameters --- */
sp->type  = IMAGERASTER;		/* set type as constructed image */
sp->size  = size;			/* set given size */
sp->baseline = baseline;		/* composite scripts baseline */
rp = sp->image;				/* raster embedded in subraster */
/* --- place super/subscripts in new raster --- */
if ( issup )				/* we have a superscript */
 rastput(rp,supsp->image,0,0,1);	/* it goes in upper-left corner */
if ( issub )				/* we have a subscript */
 rastput(rp,subsp->image,height-subht,0,1); /*in lower-left corner*/
/* -------------------------------------------------------------------------
free unneeded component subrasters and return final result to caller
-------------------------------------------------------------------------- */
end_of_job:
  if ( issub ) delete_subraster(subsp);	/* free unneeded subscript */
  if ( issup ) delete_subraster(supsp);	/* and superscript */
  return ( sp );
} /* --- end-of-function rastscripts() --- */


/* ==========================================================================
 * Function:	rastdispmath ( expression, size, sp )
 * Purpose:	displaymath handler, returns sp along with
 *		its immediately following super/subscripts
 * --------------------------------------------------------------------------
 * Arguments:	expression (I/O) char **  to first char of null-terminated
 *				string immediately following sp to be
 *				rasterized along with its super/subscripts,
 *				and returning ptr immediately following last
 *				character processed.
 *		size (I)	int containing 0-7 default font size
 *		sp (I)		subraster *  to display math operator
 *				to which super/subscripts will be added
 * --------------------------------------------------------------------------
 * Returns:	( subraster * )	ptr to subraster corresponding to sp
 *				plus its scripts, or NULL for any error
 * --------------------------------------------------------------------------
 * Notes:     o	sp returned unchanged if no super/subscript(s) follow it.
 * ======================================================================= */
/* --- entry point --- */
subraster *rastdispmath ( char **expression, int size, subraster *sp )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
char	*texscripts(),			/* parse expression for scripts */
	subscript[512], supscript[512];	/* scripts parsed from expression */
int	issub=0, issup=0;		/* true if we have sub,sup */
subraster *rasterize(), *subsp=NULL, *supsp=NULL, /* rasterize scripts */
	*rastack(),			/* stack operator with scripts */
	*new_subraster();		/* for dummy base sp, if needed */
int	vspace = 1;			/* vertical space between scripts */
/* -------------------------------------------------------------------------
Obtain subscript and/or superscript expressions, and rasterize them/it
-------------------------------------------------------------------------- */
/* --- parse for sub,superscript(s), and bump expression past it(them) --- */
if ( expression == NULL ) goto end_of_job; /* no *ptr given */
if ( *expression == NULL ) goto end_of_job; /* no expression given */
if ( *(*expression) == '\000' ) goto end_of_job; /* nothing in expression */
*expression = texscripts(*expression,subscript,supscript,3);
/* --- rasterize scripts --- */
if ( *subscript != '\000' )		/* have a subscript */
  subsp = rasterize(subscript,size-1);	/* so rasterize it at size-1 */
if ( *supscript != '\000' )		/* have a superscript */
  supsp = rasterize(supscript,size-1);	/* so rasterize it at size-1 */
/* --- set flags for convenience --- */
issub  = (subsp != (subraster *)NULL);	/* true if we have subscript */
issup  = (supsp != (subraster *)NULL);	/* true if we have superscript */
if (!issub && !issup) goto end_of_job;	/*return operator alone if neither*/
/* -------------------------------------------------------------------------
stack operator and its script(s)
-------------------------------------------------------------------------- */
/* --- stack superscript atop operator --- */
if ( issup ) {				/* we have a superscript */
 if ( sp == NULL )			/* but no base expression */
  sp = supsp;				/* so just use superscript */
 else					/* have base and superscript */
  if ( (sp=rastack(sp,supsp,1,vspace,1,3)) /* stack supsp atop base sp */
  ==   NULL ) goto end_of_job; }	/* and quit if failed */
/* --- stack operator+superscript atop subscript --- */
if ( issub ) {				/* we have a subscript */
 if ( sp == NULL )			/* but no base expression */
  sp = subsp;				/* so just use subscript */
 else					/* have base and subscript */
  if ( (sp=rastack(subsp,sp,2,vspace,1,3)) /* stack sp atop base subsp */
  ==   NULL ) goto end_of_job; }	/* and quit if failed */
sp->type = IMAGERASTER;			/* flip type of composite object */
sp->size = size;			/* and set font size */
/* -------------------------------------------------------------------------
free unneeded component subrasters and return final result to caller
-------------------------------------------------------------------------- */
end_of_job:
  return ( sp );
} /* --- end-of-function rastdispmath() --- */


/* ==========================================================================
 * Function:	rastleft ( expression, size, basesp, ildelim, arg2, arg3 )
 * Purpose:	\left...\right handler, returns a subraster corresponding to
 *		delimited subexpression at font size
 * --------------------------------------------------------------------------
 * Arguments:	expression (I)	char **  to first char of null-terminated
 *				string beginning with a \left
 *				to be rasterized
 *		size (I)	int containing 0-7 default font size
 *		basesp (I)	subraster *  to character (or subexpression)
 *				immediately preceding leading left{
 *				(unused, but passed for consistency)
 *		ildelim (I)	int containing ldelims[] index of
 *				left delimiter
 *		arg2 (I)	int unused
 *		arg3 (I)	int unused
 * --------------------------------------------------------------------------
 * Returns:	( subraster * )	ptr to subraster corresponding to subexpr,
 *				or NULL for any parsing error
 * --------------------------------------------------------------------------
 * Notes:     o
 * ======================================================================= */
/* --- entry point --- */
subraster *rastleft ( char **expression, int size, subraster *basesp,
			int ildelim, int arg2, int arg3 )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
subraster *rasterize(), *sp=NULL;	/*rasterize between \left...\right*/
subraster *get_delim(), *lp=NULL, *rp=NULL; /* left and right delim chars */
subraster *rastlimits();		/*handle sub/super scripts on lp,rp*/
subraster *rastcat();			/* concat lp||sp||rp subrasters */
int	family=CMSYEX,			/* get_delim() family */
	height=0, rheight=0,		/* subexpr, right delim height */
	margin=(size+1),		/* delim height margin over subexpr*/
	opmargin=(5);			/* extra margin for \int,\sum,\etc */
char	/* *texleft(),*/ subexpr[MAXSUBXSZ+1];/*chars between \left...\right*/
char	*texchar(),			/* get delims after \left,\right */
	ldelim[256]=".", rdelim[256]="."; /* delims following \left,\right */
char	*strtexchr(), *pleft, *pright;	/*locate \right matching our \left*/
int	isleftdot=0, isrightdot=0;	/* true if \left. or \right. */
int	isleftscript=0, isrightscript=0; /* true if delims are scripted */
int	sublen=0;			/* strlen(subexpr) */
int	idelim=0;			/* 1=left,2=right */
/* int	gotldelim = 0; */		/* true if ildelim given by caller */
int	delete_subraster();		/* free subraster if rastleft fails*/
int	wasdisplaystyle = isdisplaystyle; /* save current displaystyle */
int	istextleft=0, istextright=0;	/* true for non-displaystyle delims*/
/* --- recognized delimiters --- */
static	char left[16]="\\left", right[16]="\\right"; /* tex delimiters */
static	char *ldelims[] = {
   "unused", ".",			/* 1   for \left., \right. */
	"(", ")",			/* 2,3 for \left(, \right) */
	"\\{","\\}",			/* 4,5 for \left\{, \right\} */
	"[", "]",			/* 6,7 for \left[, \right] */
	"<", ">",			/* 8,9 for \left<, \right> */
	"|", "\\|",			/* 10,11 for \left,\right |,\|*/
	NULL };
/* --- recognized operator delimiters --- */
static	char *opdelims[] = {		/* operator delims from cmex10 */
     "int",	  "sum",	"prod",
     "cup",	  "cap",	"dot",
     "plus",	  "times",	"wedge",
     "vee",
     NULL }; /* --- end-of-opdelims[] --- */
/* --- delimiter xlation --- */
static	char *xfrom[] =			/* xlate any delim suffix... */
   { "\\|",				/* \| */
     "\\{",				/* \{ */
     "\\}",				/* \} */
     "\\lbrace",			/* \lbrace */
     "\\rbrace",			/* \rbrace */
     "\\langle",			/* \langle */
     "\\rangle",			/* \rangle */
     NULL } ; /* --- end-of-xfrom[] --- */
static	char *xto[] =			/* ...to this instead */
   { "=",				/* \| to = */
     "{",				/* \{ to { */
     "}",				/* \} to } */
     "{",				/* \lbrace to { */
     "}",				/* \rbrace to } */
     "<",				/* \langle to < */
     ">",				/* \rangle to > */
     NULL } ; /* --- end-of-xto[] --- */
/* --- non-displaystyle delimiters --- */
static	char *textdelims[] =		/* these delims _aren't_ display */
   { "|", "=",
     "(", ")",
     "[", "]",
     "<", ">",
     "{", "}",
     "dbl",				/* \lbrackdbl and \rbrackdbl */
     NULL } ; /* --- end-of-textdelims[] --- */
/* -------------------------------------------------------------------------
initialization
-------------------------------------------------------------------------- */
/* --- check args --- */
if ( *(*expression) == '\000' ) goto end_of_job; /* nothing after \left */
/* --- determine left delimiter, and set default \right. delimiter --- */
if ( ildelim!=NOVALUE && ildelim>=1 )	/* called with explicit left delim */
 { strcpy(ldelim,ldelims[ildelim]);	/* so just get a local copy */
   /* gotldelim = 1; */ }		/* and set flag that we got it */
else					/* trapped \left without delim */
 { skipwhite(*expression);		/* interpret \left ( as \left( */
   if ( *(*expression) == '\000' )	/* end-of-string after \left */
      goto end_of_job;			/* so return NULL */
   *expression = texchar(*expression,ldelim); /*pull delim from expression*/
   if ( *expression == NULL		/* probably invalid end-of-string */
   ||   *ldelim == '\000' ) goto end_of_job; } /* no delimiter */
strcpy(rdelim,".");			/* init default \right. delim */
/* -------------------------------------------------------------------------
locate \right balancing our opening \left
-------------------------------------------------------------------------- */
/* --- first \right following \left --- */
if ( (pright=strtexchr(*expression,right)) /* look for \right after \left */
!=   NULL ) {				/* found it */
 /* --- find matching \right by pushing past any nested \left's --- */
 pleft = *expression;			/* start after first \left( */
 while ( 1 ) {				/*break when matching \right found*/
  /* -- locate next nested \left if there is one --- */
  if ( (pleft=strtexchr(pleft,left))	/* find next \left */
  ==   NULL ) break;			/*no more, so matching \right found*/
  pleft += strlen(left);		/* push ptr past \left token */
  if ( pleft >= pright ) break;		/* not nested if \left after \right*/
  /* --- have nested \left, so push forward to next \right --- */
  if ( (pright=strtexchr(pright+strlen(right),right)) /* find next \right */
  ==   NULL ) break;			/* ran out of \right's */
  } /* --- end-of-while(1) --- */
 } /* --- end-of-if(pright!=NULL) --- */
/* -------------------------------------------------------------------------
push past \left(_a^b sub/superscripts, if present
-------------------------------------------------------------------------- */
pleft = *expression;			/*reset pleft after opening \left( */
if ( (lp=rastlimits(expression,size,lp)) /*dummy call push expression past b*/
!=   NULL )				/* found actual _a^b scripts, too */
  { delete_subraster(lp);		/* but we don't need them */
    lp = NULL; }			/* reset pointer, too */
/* -------------------------------------------------------------------------
get \right delimiter and subexpression between \left...\right, xlate delims
-------------------------------------------------------------------------- */
/* --- get delimiter following \right --- */
if ( pright == (char *)NULL ) {		/* assume \right. at end of exprssn*/
  strcpy(rdelim,".");			/* set default \right. */
  sublen = strlen(*expression);		/* use entire remaining expression */
  memcpy(subexpr,*expression,sublen);	/* copy all remaining chars */
  *expression += sublen; }		/* and push expression to its null */
else {					/* have explicit matching \right */
  sublen = (int)(pright-(*expression));	/* #chars between \left...\right */
  memcpy(subexpr,*expression,sublen);	/* copy chars preceding \right */
  *expression = pright+strlen(right);	/* push expression past \right */
  skipwhite(*expression);		/* interpret \right ) as \right) */
  *expression = texchar(*expression,rdelim); /*pull delim from expression*/
  if ( *rdelim == '\000' ) strcpy(rdelim,"."); } /* \right. if no rdelim */
/* --- get subexpression between \left...\right --- */
if ( sublen < 1 ) goto end_of_job;	/* nothing between delimiters */
subexpr[sublen] = '\000';		/* and null-terminate it */
/* --- adjust margin for expressions containing \middle's --- */
if ( strtexchr(subexpr,"\\middle") != NULL ) /* have enclosed \middle's */
  margin = 1;				/* so don't "overwhelm" them */
/* --- check for operator delimiter --- */
for ( idelim=0; opdelims[idelim]!=NULL; idelim++ )
  if ( strstr(ldelim,opdelims[idelim]) != NULL ) /* found operator */
    { margin += opmargin;		/* extra height for operator */
      if ( *ldelim == '\\' )		/* have leading escape */
	{strsqueeze(ldelim,1);}		/* squeeze it out */
      break; }				/* no need to check rest of table */
/* --- xlate delimiters and check for textstyle --- */
for ( idelim=1; idelim<=2; idelim++ ) {	/* 1=left, 2=right */
  char	*lrdelim  = (idelim==1? ldelim:rdelim); /* ldelim or rdelim */
  int	ix;  char *xdelim;		/* xfrom[] and xto[] index, delim */
  for( ix=0; (xdelim=xfrom[ix]) != NULL; ix++ )
    if ( strcmp(lrdelim,xdelim) == 0 )	/* found delim to xlate */
      {	strcpy(lrdelim,xto[ix]);	/* replace with corresponding xto[]*/
	break; }			/* no need to check further */
  for( ix=0; (xdelim=textdelims[ix]) != NULL; ix++ )
    if ( strstr(lrdelim,xdelim) != 0 )	/* found textstyle delim */
      {	if ( idelim == 1 )		/* if it's the \left one */
	  istextleft = 1;		/* set left textstyle flag */
	else istextright = 1;		/* else set right textstyle flag */
	break; }			/* no need to check further */
  } /* --- end-of-for(idelim) --- */
/* --- debugging --- */
if ( msgfp!=NULL && msglevel>=99 )
  fprintf(msgfp,"rastleft> left=\"%s\" right=\"%s\" subexpr=\"%s\"\n",
  ldelim,rdelim,subexpr);
/* -------------------------------------------------------------------------
rasterize subexpression
-------------------------------------------------------------------------- */
/* --- rasterize subexpression --- */
if ( (sp = rasterize(subexpr,size))	/* rasterize chars between delims */
==   NULL ) goto end_of_job;		/* quit if failed */
height = (sp->image)->height;		/* height of subexpr raster */
rheight = height+margin;		/*default rheight as subexpr height*/
/* -------------------------------------------------------------------------
rasterize delimiters, reset baselines, and add  sub/superscripts if present
-------------------------------------------------------------------------- */
/* --- check for dot delimiter --- */
isleftdot  = (strchr(ldelim,'.')!=NULL); /* true if \left. */
isrightdot = (strchr(rdelim,'.')!=NULL); /* true if \right. */
/* --- get rasters for best-fit delim characters, add sub/superscripts --- */
isdisplaystyle = (istextleft?0:9);	/* force \displaystyle */
if ( !isleftdot )			/* if not \left. */
 { /* --- first get requested \left delimiter --- */
   lp = get_delim(ldelim,rheight,family); /* get \left delim char */
   /* --- reset lp delim baseline to center delim on subexpr raster --- */
   if ( lp != NULL )			/* if get_delim() succeeded */
    { int lheight = (lp->image)->height; /* actual height of left delim */
      lp->baseline = sp->baseline + (lheight - height)/2;
      if ( lheight > rheight )		/* got bigger delim than requested */
	rheight = lheight-1; }		/* make sure right delim matches */
   /* --- then add on any sub/superscripts attached to \left( --- */
   lp = rastlimits(&pleft,size,lp);	/*\left(_a^b and push pleft past b*/
   isleftscript = isscripted; }		/* check if left delim scripted */
isdisplaystyle = (istextright?0:9);	/* force \displaystyle */
if ( !isrightdot )			/* and if not \right. */
 { /* --- first get requested \right delimiter --- */
   rp = get_delim(rdelim,rheight,family); /* get \right delim char */
   /* --- reset rp delim baseline to center delim on subexpr raster --- */
   if ( rp != NULL )			/* if get_delim() succeeded */
     rp->baseline = sp->baseline + ((rp->image)->height - height)/2;
   /* --- then add on any sub/superscripts attached to \right) --- */
   rp = rastlimits(expression,size,rp);	/*\right)_c^d, expression past d*/
   isrightscript = isscripted; }	/* check if right delim scripted */
isdisplaystyle = wasdisplaystyle;	/* original \displystyle default */
/* --- check that we got delimiters --- */
if ( 0 )
 if ( (lp==NULL && !isleftdot)		/* check that we got left( */
 ||   (rp==NULL && !isrightdot) )	/* and right) if needed */
  { if ( lp != NULL ) free ((void *)lp); /* free \left-delim subraster */
    if ( rp != NULL ) free ((void *)rp); /* and \right-delim subraster */
    if (0) { delete_subraster(sp);	/* if failed, free subraster */
             sp = (subraster *)NULL; }	/* signal error to caller */
    goto end_of_job; }			/* and quit */
/* -------------------------------------------------------------------------
concat  lp || sp || rp  components
-------------------------------------------------------------------------- */
/* --- concat lp||sp||rp to obtain final result --- */
if ( lp != NULL )			/* ignore \left. */
  sp = rastcat(lp,sp,3);		/* concat lp||sp and free sp,lp */
if ( sp != NULL )			/* succeeded or ignored \left. */
  if ( rp != NULL )			/* ignore \right. */
    sp = rastcat(sp,rp,3);		/* concat sp||rp and free sp,rp */
/* --- back to caller --- */
end_of_job:
  isdelimscript = isrightscript;	/* signal if right delim scripted */
  return ( sp );
} /* --- end-of-function rastleft() --- */


/* ==========================================================================
 * Function:	rastright ( expression, size, basesp, ildelim, arg2, arg3 )
 * Purpose:	...\right handler, intercepts an unexpected/unbalanced \right
 * --------------------------------------------------------------------------
 * Arguments:	expression (I)	char **  to first char of null-terminated
 *				string beginning with a \right
 *				to be rasterized
 *		size (I)	int containing 0-7 default font size
 *		basesp (I)	subraster *  to character (or subexpression)
 *				immediately preceding leading left{
 *				(unused, but passed for consistency)
 *		ildelim (I)	int containing rdelims[] index of
 *				right delimiter
 *		arg2 (I)	int unused
 *		arg3 (I)	int unused
 * --------------------------------------------------------------------------
 * Returns:	( subraster * )	ptr to subraster corresponding to subexpr,
 *				or NULL for any parsing error
 * --------------------------------------------------------------------------
 * Notes:     o
 * ======================================================================= */
/* --- entry point --- */
subraster *rastright ( char **expression, int size, subraster *basesp,
			int ildelim, int arg2, int arg3 )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
subraster /* *rasterize(),*/ *sp=NULL;	/*rasterize \right subexpr's*/
  if ( sp != NULL )			/* returning entire expression */
    {
      isreplaceleft = 1;		/* set flag to replace left half*/
    }
return ( sp );
} /* --- end-of-function rastright() --- */


/* ==========================================================================
 * Function:	rastmiddle ( expression, size, basesp,  arg1, arg2, arg3 )
 * Purpose:	\middle handler, returns subraster corresponding to
 *		entire expression with \middle delimiter(s) sized to fit.
 * --------------------------------------------------------------------------
 * Arguments:	expression (I/O) char **  to first char of null-terminated
 *				string immediately following \middle to be
 *				rasterized, and returning ptr immediately
 *				to terminating null.
 *		size (I)	int containing 0-7 default font size
 *		basesp (I)	subraster *  to character (or subexpression)
 *				immediately preceding \middle
 *				(unused, but passed for consistency)
 *		arg1 (I)	int unused
 *		arg2 (I)	int unused
 *		arg3 (I)	int unused
 * --------------------------------------------------------------------------
 * Returns:	( subraster * )	ptr to subraster corresponding to expression,
 *				or NULL for any parsing error
 *				(expression ptr unchanged if error occurs)
 * --------------------------------------------------------------------------
 * Notes:     o
 * ======================================================================= */
/* --- entry point --- */
subraster *rastmiddle ( char **expression, int size, subraster *basesp,
			int arg1, int arg2, int arg3 )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
subraster *rasterize(), *sp=NULL, *subsp[32]; /*rasterize \middle subexpr's*/
char	*exprptr = *expression,		/* local copy of ptr to expression */
	*texchar(), delim[32][132],	/* delimiters following \middle's */
	*strtexchr(),			/* locate \middle's */
	subexpr[MAXSUBXSZ+1], *subptr=NULL;/*subexpression between \middle's*/
int	height=0, habove=0, hbelow=0;	/* height, above & below baseline */
int	idelim, ndelims=0,		/* \middle count (max 32) */
	family = CMSYEX;		/* delims from CMSY10 or CMEX10 */
subraster *subrastcpy(),		/* copy subraster */
	*rastcat(),			/* concatanate subraster */
	*get_delim();			/* get rasterized delimiter */
int	delete_subraster();		/* free work area subsp[]'s at eoj */
/* -------------------------------------------------------------------------
initialization
-------------------------------------------------------------------------- */
subsp[0] = leftexpression;		/* expressn preceding 1st \middle */
subsp[1] = NULL;			/* set first null */
/* -------------------------------------------------------------------------
accumulate subrasters between consecutive \middle\delim...\middle\delim...'s
-------------------------------------------------------------------------- */
while ( ndelims < 30 )			/* max of 31 \middle's */
  {
  /* --- maintain max height above,below baseline --- */
  if ( subsp[ndelims] != NULL )		/*exprssn preceding current \middle*/
   { int baseline = (subsp[ndelims])->baseline;  /* #rows above baseline */
     height = ((subsp[ndelims])->image)->height; /* tot #rows (height) */
     habove = max2(habove,baseline);	/* max #rows above baseline */
     hbelow = max2(hbelow,height-baseline); } /* max #rows below baseline */
  /* --- get delimter after \middle --- */
  skipwhite(exprptr);			/*skip space betwn \middle & \delim*/
  exprptr = texchar(exprptr,delim[ndelims]); /* \delim after \middle */
  if ( *(delim[ndelims]) == '\000' )	/* \middle at end-of-expression */
    break;				/* ignore it and consider job done */
  ndelims++;				/* count another \middle\delim */
  /* --- get subexpression between \delim and next \middle --- */
  subsp[ndelims] = NULL;		/* no subexpresion yet */
  if ( *exprptr == '\000' )		/* end-of-expression after \delim */
    break;				/* so we have all subexpressions */
  if ( (subptr = strtexchr(exprptr,"\\middle")) /* find next \middle */
  ==   NULL )				/* no more \middle's */
   { strncpy(subexpr,exprptr,MAXSUBXSZ); /*get entire remaining expression*/
     subexpr[MAXSUBXSZ] = '\000';	/* make sure it's null-terminated */
     exprptr += strlen(exprptr); }	/* push exprptr to terminating '\0'*/
  else					/* have another \middle */
   { int sublen = (int)(subptr-exprptr); /* #chars between \delim...\middle*/
     memcpy(subexpr,exprptr,min2(sublen,MAXSUBXSZ)); /* get subexpression */
     subexpr[min2(sublen,MAXSUBXSZ)] = '\000'; /* and null-terminate it */
     exprptr += (sublen+strlen("\\middle")); } /* push exprptr past \middle*/
  /* --- rasterize subexpression --- */
  subsp[ndelims] = rasterize(subexpr,size); /* rasterize subexpresion */
  } /* --- end-of-while(1) --- */
/* -------------------------------------------------------------------------
construct \middle\delim's and concatanate them between subexpressions
-------------------------------------------------------------------------- */
if ( ndelims < 1			/* no delims */
||   (height=habove+hbelow) < 1 )	/* or no subexpressions? */
  goto end_of_job;			/* just flush \middle directive */
for ( idelim=0; idelim<=ndelims; idelim++ )
  {
  /* --- first add on subexpression preceding delim --- */
  if ( subsp[idelim] != NULL ) {	/* have subexpr preceding delim */
    if ( sp == NULL )			/* this is first piece */
     { sp = subsp[idelim];		/* so just use it */
       if ( idelim == 0 ) sp = subrastcpy(sp); } /* or copy leftexpression */
    else sp = rastcat(sp,subsp[idelim],(idelim>0?3:1)); } /* or concat it */
  /* --- now construct delimiter --- */
  if ( *(delim[idelim]) != '\000' )	/* have delimter */
   { subraster *delimsp = get_delim(delim[idelim],height,family);
     if ( delimsp != NULL )		/* rasterized delim */
      {	delimsp->baseline = habove;	/* set baseline */
	if ( sp == NULL )		/* this is first piece */
	  sp = delimsp;			/* so just use it */
	else sp = rastcat(sp,delimsp,3); } } /*or concat to existing pieces*/
  } /* --- end-of-for(idelim) --- */
/* --- back to caller --- */
end_of_job:
  if ( 0 ) /* now handled above */
    for ( idelim=1; idelim<=ndelims; idelim++ ) /* free subsp[]'s (not 0) */
     if ( subsp[idelim] != NULL )	/* have allocated subraster */
      delete_subraster(subsp[idelim]);	/* so free it */
  if ( sp != NULL )			/* returning entire expression */
    { int newht = (sp->image)->height;	/* height of returned subraster */
      sp->baseline = min2(newht-1,newht/2+5); /* guess new baseline */
      isreplaceleft = 1;		/* set flag to replace left half*/
      *expression += strlen(*expression); } /* and push to terminating null*/
  return ( sp );
} /* --- end-of-function rastmiddle() --- */


/* ==========================================================================
 * Function:	rastflags ( expression, size, basesp,  flag, value, arg3 )
 * Purpose:	sets an internal flag, e.g., for \rm, or sets an internal
 *		value, e.g., for \unitlength=<value>, and returns NULL
 *		so nothing is displayed
 * --------------------------------------------------------------------------
 * Arguments:	expression (I)	char **  to first char of null-terminated
 *				LaTeX expression (unused/unchanged)
 *		size (I)	int containing base font size (not used,
 *				just stored in subraster)
 *		basesp (I)	subraster *  to character (or subexpression)
 *				immediately preceding "flags" directive
 *				(unused but passed for consistency)
 *		flag (I)	int containing #define'd symbol specifying
 *				internal flag to be set
 *		value (I)	int containing new value of flag
 *		arg3 (I)	int unused
 * --------------------------------------------------------------------------
 * Returns:	( subraster * )	NULL so nothing is displayed
 * --------------------------------------------------------------------------
 * Notes:     o
 * ======================================================================= */
/* --- entry point --- */
subraster *rastflags ( char **expression, int size, subraster *basesp,
			int flag, int value, int arg3 )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
char	*texsubexpr(),			/* parse expression for... */
	valuearg[1024]="NOVALUE";	/* value from expression, if needed */
int	argvalue=NOVALUE,		/* atoi(valuearg) */
	isdelta=0,			/* true if + or - precedes valuearg */
	valuelen=0;			/* strlen(valuearg) */
double	dblvalue=(-99.), strtod();	/*convert ascii {valuearg} to double*/
static	int displaystylelevel = (-99);	/* \displaystyle set at recurlevel */
/* -------------------------------------------------------------------------
set flag or value
-------------------------------------------------------------------------- */
switch ( flag )
  {
  default: break;			/* unrecognized flag */
  case ISFONTFAM:
    if ( isthischar((*(*expression)),WHITEMATH) ) /* \rm followed by white */
      (*expression)++;			/* skip leading ~ after \rm */
    fontnum = value;			/* set font family */
    break;
  case ISSTRING: isstring=value; break;	/* set string/image mode */
  case ISDISPLAYSTYLE:			/* set \displaystyle mode */
    displaystylelevel = recurlevel;	/* \displaystyle set at recurlevel */
    isdisplaystyle=value; break;
  case ISOPAQUE:  istransparent=value; break; /* set transparent/opaque */
  case ISREVERSE:			/* reverse video */
    if ( value==1 || value==NOVALUE )
      {	fgred=255-fgred; fggreen=255-fggreen; fgblue=255-fgblue; }
    if ( value==2 || value==NOVALUE )
      {	bgred=255-bgred; bggreen=255-bggreen; bgblue=255-bgblue; }
    if ( value==2 || value==NOVALUE )
      isblackonwhite = !isblackonwhite;
    if ( gammacorrection > 0.0001 )	/* have gamma correction */
      gammacorrection = REVERSEGAMMA;	/* use reverse video gamma instead */
    break;
  case ISSUPER:				/* set supersampling/lowpass flag */
    #ifndef SSFONTS			/* don't have ss fonts loaded */
      value = 0;			/* so force lowpass */
    #endif
    isss = issupersampling = value;
    fonttable = (issupersampling?ssfonttable:aafonttable); /* set fonts */
    break;
  case ISFONTSIZE:			/* set fontsize */
  case ISMAGSTEP:			/* set magstep */
  case ISDISPLAYSIZE:			/* set displaysize */
  case ISCONTENTTYPE:			/*enable/disable content-type lines*/
  case ISCONTENTCACHED:			/* write content-type to cache file*/
  case ISSHRINK:			/* set shrinkfactor */
  case ISAAALGORITHM:			/* set anti-aliasing algorithm */
  case ISWEIGHT:			/* set font weight */
  case ISCENTERWT:			/* set lowpass center pixel weight */
  case ISADJACENTWT:			/* set lowpass adjacent weight */
  case ISCORNERWT:			/* set lowpass corner weight */
  case ISCOLOR:				/* set red(1),green(2),blue(3) */
  case ISSMASH:				/* set (minimum) "smash" margin */
  case ISGAMMA:				/* set gamma correction */
  case ISPBMPGM:			/* set pbmpgm output flag and ptype*/
  case ISUTHETA:			/* set utheta 3d-rotation angle */
    if ( value != NOVALUE )		/* passed a fixed value to be set */
      {	argvalue = value;		/* set given fixed int value */
	dblvalue = (double)value; }	/* or maybe interpreted as double */
    else				/* get value from expression */
      {	*expression = texsubexpr(*expression,valuearg,1023,"{","}",0,0);
	if ( *valuearg != '\000' )	/* guard against empty string */
	 if ( !isalpha(*valuearg) )	/* and against alpha string args */
	  if ( !isthischar(*valuearg,"?") ) /*leading ? is query for value*/
	   { isdelta = isthischar(*valuearg,"+-"); /* leading + or - */
	     if ( memcmp(valuearg,"--",2) == 0 ) /* leading -- signals...*/
	       { isdelta=0; strsqueeze(valuearg,1); } /* ...not delta */
	     switch ( flag ) {		/* convert to double or int */
	      default: argvalue = atoi(valuearg); break; /* convert to int */
	      case ISGAMMA:
	      case ISUTHETA:
		dblvalue = strtod(valuearg,NULL); break; } /* or to double */
	   } /* --- end-of-if(*valuearg!='?') --- */
      } /* --- end-of-if(value==NOVALUE) --- */
    switch ( flag )
      {
      default: break;
      case ISCOLOR:			/* set color */
	slower(valuearg);		/* convert arg to lower case */
	if ( argvalue==1 || strstr(valuearg,"red") )
	  { fggreen = fgblue = (isblackonwhite?0:255);
	    fgred = (isblackonwhite?255:0); }
	if ( argvalue==2 || strstr(valuearg,"green") )
	  { fgred = fgblue = (isblackonwhite?0:255);
	    fggreen = (isblackonwhite?255:0); }
	if ( argvalue==3 || strstr(valuearg,"blue") )
	  { fgred = fggreen = (isblackonwhite?0:255);
	    fgblue = (isblackonwhite?255:0); }
	if ( argvalue==0 || strstr(valuearg,"black") )
	    fgred = fggreen = fgblue = (isblackonwhite?0:255);
	if ( argvalue==7 || strstr(valuearg,"white") )
	    fgred = fggreen = fgblue = (isblackonwhite?255:0);
	break;
      case ISFONTSIZE:			/* set fontsize */
	if ( argvalue != NOVALUE )	/* got a value */
	  { int largestsize = (issupersampling?16:LARGESTSIZE);
	    fontsize = (isdelta? fontsize+argvalue : argvalue);
	    fontsize = max2(0,min2(fontsize,largestsize));
	    shrinkfactor = shrinkfactors[fontsize];
	    if ( isdisplaystyle == 1	/* displaystyle enabled but not set*/
	    ||  (1 && isdisplaystyle==2) /* displaystyle enabled and set */
	    ||  (0 && isdisplaystyle==0) )/*\textstyle disabled displaystyle*/
	     if ( displaystylelevel != recurlevel ) /*respect \displaystyle*/
	      if ( !ispreambledollars )	{ /* respect $$...$$'s */
	       if ( fontsize >= displaysize )
		isdisplaystyle = 2;	/* forced */
	       else isdisplaystyle = 1; }
	    /*displaystylelevel = (-99);*/ } /* reset \displaystyle level */
	else				/* embed font size in expression */
	  { sprintf(valuearg,"%d",fontsize); /* convert size */
	    valuelen = strlen(valuearg); /* ought to be 1 */
	    if ( *expression != '\000' ) /* ill-formed expression */
	     { *expression = (char *)(*expression-valuelen); /*back up buff*/
	       memcpy(*expression,valuearg,valuelen); } } /*and put in size*/
	break;
      case ISMAGSTEP:			/* set magstep */
	if ( argvalue != NOVALUE ) {	/* got a value */
	  int largestmag = 10;
	  magstep = (isdelta? magstep+argvalue : argvalue);
	  magstep = max2(1,min2(magstep,largestmag)); }
	break;
      case ISDISPLAYSIZE:		/* set displaysize */
	if ( argvalue != NOVALUE )	/* got a value */
	    displaysize = (isdelta? displaysize+argvalue : argvalue);
	break;
      case ISCONTENTTYPE:		/*enable/disable content-type lines*/
	if ( argvalue != NOVALUE )	/* got a value */
	    isemitcontenttype = (argvalue>0?1:0);
	break;
      case ISCONTENTCACHED:		/* write content-type to cache file*/
	if ( argvalue != NOVALUE )	/* got a value */
	    iscachecontenttype = (argvalue>0?1:0);
	break;
      case ISPBMPGM:			/* output pbm/pgm rather than gif */
        ispbmpgm = 1;			/* always set pbm/pgm flag */
        pbmpgmtype = 1;			/* and init type (1 for pbm) */
	if ( argvalue != NOVALUE )	/* got a value */
	    pbmpgmtype = (argvalue<1?1:argvalue);
	break;
      case ISSMASH:			/* set (minimum) "smash" margin */
	if ( argvalue != NOVALUE )	/* got a value */
	  { smashmargin = argvalue;	/* set value */
	    if ( arg3 != NOVALUE ) isdelta=arg3; /* hard-coded isdelta */
	    issmashdelta = (isdelta?1:0); } /* and set delta flag */
	smashmargin = max2((isdelta?-5:0),min2(smashmargin,32)); /*sanity*/
	isexplicitsmash = 1;		/* signal explicit \smash directive*/
	break;
      case ISSHRINK:			/* set shrinkfactor */
	if ( argvalue != NOVALUE )	/* got a value */
	  shrinkfactor = (isdelta? shrinkfactor+argvalue : argvalue);
	shrinkfactor = max2(1,min2(shrinkfactor,27)); /* sanity check */
	break;
      case ISAAALGORITHM:		/* set anti-aliasing algorithm */
	if ( argvalue != NOVALUE ) {	/* got a value */
	  if ( argvalue >= 0 ) {	/* non-negative to set algorithm */
	      aaalgorithm = argvalue;	/* set algorithm number */
	    aaalgorithm = max2(0,min2(aaalgorithm,4)); } /* bounds check */
	  else maxfollow = abs(argvalue); } /* or maxfollow=abs(negative#) */
	break;
      case ISWEIGHT:			/* set font weight number */
	value =	(argvalue==NOVALUE? NOVALUE : /* don't have a value */
		(isdelta? weightnum+argvalue : argvalue));
	if ( value>=0 && value<maxaaparams ) /* in range */
	  { weightnum   = value;	/* reset weightnum index */
	    minadjacent = aaparams[weightnum].minadjacent;
	    maxadjacent = aaparams[weightnum].maxadjacent;
	    cornerwt    = aaparams[weightnum].cornerwt;
	    adjacentwt  = aaparams[weightnum].adjacentwt;
	    centerwt    = aaparams[weightnum].centerwt;
	    fgalias     = aaparams[weightnum].fgalias;
	    fgonly      = aaparams[weightnum].fgonly;
	    bgalias     = aaparams[weightnum].bgalias;
	    bgonly      = aaparams[weightnum].bgonly; }
	break;
      case ISCENTERWT:			/* set lowpass center pixel weight */
	if ( argvalue != NOVALUE )	/* got a value */
	  centerwt = argvalue;		/* set lowpass center weight */
	break;
      case ISADJACENTWT:		/* set lowpass adjacent weight */
	if ( argvalue != NOVALUE )	/* got a value */
	  adjacentwt = argvalue;	/* set lowpass adjacent weight */
	break;
      case ISCORNERWT:			/* set lowpass corner weight */
	if ( argvalue != NOVALUE )	/* got a value */
	  cornerwt = argvalue;		/* set lowpass corner weight */
	break;
      case ISGAMMA:			/* set gamma correction */
	if ( dblvalue >= 0.0 )		/* got a value */
	  gammacorrection = dblvalue;	/* set gamma correction */
	break;
      case ISUTHETA:			/* set 3d-rotation angle for chars */
	if ( absval(dblvalue) >= 1.0e-6 ) /* got a value */
	  utheta = dblvalue;		/* set utheta */
	break;
      } /* --- end-of-switch() --- */
    break;
  case PNMPARAMS:			/*set fgalias,fgonly,bgalias,bgonly*/
    *expression = texsubexpr(*expression,valuearg,1023,"{","}",0,0);
    valuelen = strlen(valuearg);	/* ought to be 1-4 */
    if ( valuelen>0 && isthischar(toupper(valuearg[0]),"TY1") ) fgalias=1;
    if ( valuelen>0 && isthischar(toupper(valuearg[0]),"FN0") ) fgalias=0;
    if ( valuelen>1 && isthischar(toupper(valuearg[1]),"TY1") ) fgonly =1;
    if ( valuelen>1 && isthischar(toupper(valuearg[1]),"FN0") ) fgonly =0;
    if ( valuelen>2 && isthischar(toupper(valuearg[2]),"TY1") ) bgalias=1;
    if ( valuelen>2 && isthischar(toupper(valuearg[2]),"FN0") ) bgalias=0;
    if ( valuelen>3 && isthischar(toupper(valuearg[3]),"TY1") ) bgonly =1;
    if ( valuelen>3 && isthischar(toupper(valuearg[3]),"FN0") ) bgonly =0;
    break;
  case UNITLENGTH:
    if ( value != NOVALUE )		/* passed a fixed value to be set */
	unitlength = (double)(value);	/* set given fixed value */
    else				/* get value from expression */
      {	*expression = texsubexpr(*expression,valuearg,1023,"{","}",0,0);
	if ( *valuearg != '\000' )	/* guard against empty string */
	  unitlength = strtod(valuearg,NULL); } /* convert to double */
    iunitlength = (int)(unitlength+0.5); /* iunitlength reset */
    break;
  } /* --- end-of-switch(flag) --- */
return ( NULL );			/*just set value, nothing to display*/
} /* --- end-of-function rastflags() --- */


/* ==========================================================================
 * Function:	rastspace(expression, size, basesp,  width, isfill, isheight)
 * Purpose:	returns a blank/space subraster width wide,
 *		with baseline and height corresponding to basep
 * --------------------------------------------------------------------------
 * Arguments:	expression (I)	char **  to first char of null-terminated
 *				LaTeX expression (unused/unchanged)
 *		size (I)	int containing base font size (not used,
 *				just stored in subraster)
 *		basesp (I)	subraster *  to character (or subexpression)
 *				immediately preceding space, whose baseline
 *				and height params are transferred to space
 *		width (I)	int containing #bits/pixels for space width
 *		isfill (I)	int containing true to \hfill complete
 *				expression out to width
 *				(Kludge: isfill=99 signals \hspace*
 *				for negative space)
 *		isheight (I)	int containing true (but not NOVALUE)
 *				to treat width arg as height
 * --------------------------------------------------------------------------
 * Returns:	( subraster * )	ptr to empty/blank subraster
 *				or NULL for any error
 * --------------------------------------------------------------------------
 * Notes:     o
 * ======================================================================= */
/* --- entry point --- */
subraster *rastspace ( char **expression, int size, subraster *basesp,
			int width, int isfill, int isheight )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
subraster *new_subraster(), *spacesp=NULL; /* subraster for space */
raster	*bp=NULL, *backspace_raster();	/* for negative space */
int	delete_subraster();		/* if fail, free unneeded subraster*/
int	baseht=1, baseln=0;		/* height,baseline of base symbol */
int	pixsz = 1;			/*default #bits per pixel, 1=bitmap*/
int	isstar=0, minspace=0;		/* defaults for negative hspace */
int	maxwidth = 2047;		/* max width of expression in pixels*/
char	*texsubexpr(), widtharg[256];	/* parse for optional {width} */
int	evalterm(), evalue=0;		/* evaluate [args], {args} */
subraster *rasterize(), *rightsp=NULL;	/*rasterize right half of expression*/
subraster *rastcat();			/* cat rightsp after \hfill */
/* -------------------------------------------------------------------------
initialization
-------------------------------------------------------------------------- */
if ( isfill > 1 ) { isstar=1; isfill=0; } /* large fill signals \hspace* */
if ( isfill == NOVALUE ) isfill=0;	/* novalue means false */
if ( isheight == NOVALUE ) isheight=0;	/* novalue means false */
minspace = (isstar?(-1):0);		/* reset default minspace */
/* -------------------------------------------------------------------------
determine width if not given (e.g., \hspace{width}, \hfill{width})
-------------------------------------------------------------------------- */
if ( width == 0 ) {			/* width specified in expression */
  double dwidth;  int widthval;		/* test {width} before using it */
  int minwidth = (isfill||isheight?1:-maxwidth); /* \hspace allows negative */
  /* --- check if optional [minspace] given for negative \hspace --- */
  if ( *(*expression) == '[' ) {	/* [minspace] if leading char is [ */
    /* ---parse [minspace], bump expression past it, evaluate expression--- */
    *expression = texsubexpr(*expression,widtharg,127,"[","]",0,0);
    if ( !isempty(widtharg) ) {		/* got [minspace] */
      evalue = evalterm(mimestore,widtharg); /* evaluate widtharg expr */
      minspace = iround(unitlength*((double)evalue)); } /* in pixels */
    } /* --- end-of-if(*(*expression)=='[') --- */
  width = 1;				/* set default width */
  *expression = texsubexpr(*expression,widtharg,255,"{","}",0,0);
  dwidth = unitlength*((double)evalterm(mimestore,widtharg)); /* scaled */
  widthval =				/* convert {width} to integer */
		(int)( dwidth + (dwidth>=0.0?0.5:(-0.5)) );
  if ( widthval>=minwidth && widthval<=maxwidth ) /* sanity check */
    width = widthval;			/* replace deafault width */
  } /* --- end-of-if(width==0) --- */
/* -------------------------------------------------------------------------
first check for negative space
-------------------------------------------------------------------------- */
if ( width < 0 ) {			/* have negative hspace */
 if ( leftexpression != (subraster *)NULL ) /* can't backspace */
  if ( (spacesp=new_subraster(0,0,0))	/* get new subraster for backspace */
  !=   NULL ) {				/* and if we succeed... */
   int nback=(-width), pback;		/*#pixels wanted,actually backspaced*/
   if ( (bp=backspace_raster(leftexpression->image,nback,&pback,minspace,0))
   !=    NULL ) {			/* and if backspace succeeds... */
     spacesp->image = bp;		/* save backspaced image */
     /*spacesp->type = leftexpression->type;*/ /* copy original type */
     spacesp->type = blanksignal;	/* need to propagate blanks */
     spacesp->size = leftexpression->size; /* copy original font size */
     spacesp->baseline = leftexpression->baseline; /* and baseline */
     blanksymspace += -(nback-pback);	/* wanted more than we got */
     isreplaceleft = 1; }		/*signal to replace entire expressn*/
   else {				/* backspace failed */
     delete_subraster(spacesp);		/* free unneeded envelope */
     spacesp = (subraster *)NULL; } }	/* and signal failure */
 goto end_of_job;
 } /* --- end-of-if(width<0) --- */
/* -------------------------------------------------------------------------
see if width is "absolute" or fill width
-------------------------------------------------------------------------- */
if ( isfill				/* called as \hfill{} */
&&   !isheight )			/* parameter conflict */
 { if ( leftexpression != NULL )	/* if we have left half */
    width -= (leftexpression->image)->width; /*reduce left width from total*/
   if ( (rightsp=rasterize(*expression,size)) /* rasterize right half */
   != NULL )				/* succeeded */
    width -= (rightsp->image)->width; } /* reduce right width from total */
/* -------------------------------------------------------------------------
construct blank subraster, and return it to caller
-------------------------------------------------------------------------- */
/* --- get parameters from base symbol --- */
if ( basesp != (subraster *)NULL )	/* we have base symbol for space */
  { baseht = (basesp->image)->height; 	/* height of base symbol */
    baseln =  basesp->baseline; }	/* and its baseline */
/* --- flip params for height --- */
if ( isheight )				/* width is actually height */
  { baseht = width;			/* use given width as height */
    width = 1; }			/* and set default width */
/* --- generate and init space subraster --- */
if ( width > 0 )			/*make sure we have positive width*/
 if ( (spacesp=new_subraster(width,baseht,pixsz)) /*generate space subraster*/
 !=   NULL )				/* and if we succeed... */
  { /* --- ...re-init subraster parameters --- */
    spacesp->size = size;		/*propagate base font size forward*/
    if(1)spacesp->type = blanksignal;	/* need to propagate blanks (???) */
    spacesp->baseline = baseln; }	/* ditto baseline */
/* -------------------------------------------------------------------------
concat right half if \hfill-ing
-------------------------------------------------------------------------- */
if ( rightsp != NULL )			/* we have a right half after fill */
  { spacesp = (spacesp==NULL? rightsp:	/* no space, so just use right half*/
	rastcat(spacesp,rightsp,3));	/* or cat right half after space */
    spacesp->type = blanksignal;	/* need to propagate blanks */
    *expression += strlen((*expression)); } /* push expression to its null */
end_of_job:
  return ( spacesp );
} /* --- end-of-function rastspace() --- */


/* ==========================================================================
 * Function:	rastnewline ( expression, size, basesp,  arg1, arg2, arg3 )
 * Purpose:	\\ handler, returns subraster corresponding to
 *		left-hand expression preceding \\ above right-hand expression
 * --------------------------------------------------------------------------
 * Arguments:	expression (I/O) char **  to first char of null-terminated
 *				string immediately following \\ to be
 *				rasterized, and returning ptr immediately
 *				to terminating null.
 *		size (I)	int containing 0-7 default font size
 *		basesp (I)	subraster *  to character (or subexpression)
 *				immediately preceding \\
 *				(unused, but passed for consistency)
 *		arg1 (I)	int unused
 *		arg2 (I)	int unused
 *		arg3 (I)	int unused
 * --------------------------------------------------------------------------
 * Returns:	( subraster * )	ptr to subraster corresponding to expression,
 *				or NULL for any parsing error
 *				(expression ptr unchanged if error occurs)
 * --------------------------------------------------------------------------
 * Notes:     o
 * ======================================================================= */
/* --- entry point --- */
subraster *rastnewline ( char **expression, int size, subraster *basesp,
			int arg1, int arg2, int arg3 )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
subraster *rastack(), *newlsp=NULL;	/* subraster for both lines */
subraster *rasterize(), *rightsp=NULL;	/*rasterize right half of expression*/
char	*texsubexpr(), spacexpr[129]/*, *xptr=spacexpr*/; /*for \\[vspace]*/
int	evalterm(), evalue=0;		/* evaluate [arg], {arg} */
int	vspace = size+2;		/* #pixels between lines */
/* -------------------------------------------------------------------------
obtain optional [vspace] argument immediately following \\ command
-------------------------------------------------------------------------- */
/* --- check if [vspace] given --- */
if ( *(*expression) == '[' )		/*have [vspace] if leading char is [*/
  {
  /* ---parse [vspace] and bump expression past it, interpret as double--- */
  *expression = texsubexpr(*expression,spacexpr,127,"[","]",0,0);
  if ( *spacexpr == '\000' ) goto end_of_job; /* couldn't get [vspace] */
  evalue = evalterm(mimestore,spacexpr); /* evaluate [space] arg */
  vspace = iround(unitlength*((double)evalue)); /* vspace in pixels */
  } /* --- end-of-if(*(*expression)=='[') --- */
if ( leftexpression == NULL ) goto end_of_job; /* nothing preceding \\ */
/* -------------------------------------------------------------------------
rasterize right half of expression and stack left half above it
-------------------------------------------------------------------------- */
/* --- rasterize right half --- */
if ( (rightsp=rasterize(*expression,size)) /* rasterize right half */
== NULL ) goto end_of_job;		/* quit if failed */
/* --- stack left half above it --- */
/*newlsp = rastack(rightsp,leftexpression,1,vspace,0,3);*//*right under left*/
newlsp = rastack(rightsp,leftexpression,1,vspace,0,1); /*right under left*/
/* --- back to caller --- */
end_of_job:
  if ( newlsp != NULL )			/* returning entire expression */
    { int newht = (newlsp->image)->height; /* height of returned subraster */
      newlsp->baseline = min2(newht-1,newht/2+5); /* guess new baseline */
      isreplaceleft = 1;		/* so set flag to replace left half*/
      *expression += strlen(*expression); } /* and push to terminating null*/
  return ( newlsp );			/* 1st line over 2nd, or null=error*/
} /* --- end-of-function rastnewline() --- */


/* ==========================================================================
 * Function:	rastarrow ( expression, size, basesp,  drctn, isBig, arg3 )
 * Purpose:	returns left/right arrow subraster (e.g., for \longrightarrow)
 * --------------------------------------------------------------------------
 * Arguments:	expression (I)	char **  to first char of null-terminated
 *				LaTeX expression (unused/unchanged)
 *		size (I)	int containing base font size (not used,
 *				just stored in subraster)
 *		basesp (I)	subraster *  to character (or subexpression)
 *				immediately preceding space, whose baseline
 *				and height params are transferred to space
 *		drctn (I)	int containing +1 for right, -1 for left,
 *				or 0 for leftright
 *		isBig (I)	int containing 0 for ---> or 1 for ===>
 *		arg3 (I)	int unused
 * --------------------------------------------------------------------------
 * Returns:	( subraster * )	ptr to left/right arrow subraster
 *				or NULL for any error
 * --------------------------------------------------------------------------
 * Notes:     o	An optional argument [width] may *immediately* follow
 *		the \longxxx to explicitly set the arrow's width in pixels.
 *		For example, \longrightarrow calculates a default width
 *		(as usual in LaTeX), whereas \longrightarrow[50] explicitly
 *		draws a 50-pixel long arrow.  This can be used, e.g.,
 *		to draw commutative diagrams in conjunction with
 *		\array (and maybe with \stackrel and/or \relstack, too).
 *	      o	In case you really want to render, say, [f]---->[g], just
 *		use an intervening space, i.e., [f]\longrightarrow~[g].
 *		In text mode use two spaces {\rm~[f]\longrightarrow~~[g]}.
 * ======================================================================= */
/* --- entry point --- */
subraster *rastarrow ( char **expression, int size, subraster *basesp,
			int drctn, int isBig, int arg3 )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
subraster *arrow_subraster(), *arrowsp=NULL; /* subraster for arrow */
char	*texsubexpr(), widtharg[256];	/* parse for optional [width] */
char	*texscripts(), sub[1024],super[1024]; /* and _^limits after [width]*/
subraster *rasterize(), *subsp=NULL,*supsp=NULL; /*rasterize limits*/
subraster *new_subraster(), *rastack(), *spacesp=NULL; /*space below arrow*/
int	delete_subraster();		/*free work areas in case of error*/
int	evalterm();			/* evaluate [arg], {arg} */
int	width = 10 + 8*size,  height;	/* width, height for \longxxxarrow */
int	maxwidth = 1024;		/* max arrow width in pixels */
int	islimits = 1;			/*true to handle limits internally*/
int	limsize = size-1;		/* font size for limits */
int	vspace = 1;			/* #empty rows below arrow */
int	pixsz = 1;			/*default #bits per pixel, 1=bitmap*/
/* -------------------------------------------------------------------------
construct longleft/rightarrow subraster, with limits, and return it to caller
-------------------------------------------------------------------------- */
/* --- check for optional width arg and replace default width --- */
if ( *(*expression) == '[' )		/*check for []-enclosed optional arg*/
  { int widthval;			/* test [width] before using it */
    *expression = texsubexpr(*expression,widtharg,255,"[","]",0,0);
    widthval =				/* convert [width] to integer */
	(int)((unitlength*((double)evalterm(mimestore,widtharg)))+0.5);
    if ( widthval>=2 && widthval<=maxwidth )	/* sanity check */
      width = widthval; }		/* replace deafault width */
/* --- now parse for limits, and bump expression past it(them) --- */
if ( islimits )				/* handling limits internally */
  { *expression = texscripts(*expression,sub,super,3); /* parse for limits */
    if ( *sub != '\000' )		/*have a subscript following arrow*/
      subsp = rasterize(sub,limsize);	/* so try to rasterize subscript */
    if ( *super != '\000' )		/*have superscript following arrow*/
      supsp = rasterize(super,limsize); } /*so try to rasterize superscript*/
/* --- set height based on width --- */
height = min2(17,max2(9,(width+2)/6));	/* height based on width */
height = 1 + (height/2)*2;		/* always force odd height */
/* --- generate arrow subraster --- */
if ( (arrowsp=arrow_subraster(width,height,pixsz,drctn,isBig)) /*build arrow*/
==   NULL ) goto end_of_job;		/* and quit if we failed */
/* --- add space below arrow --- */
if ( vspace > 0 )			/* if we have space below arrow */
  if ( (spacesp=new_subraster(width,vspace,pixsz)) /*allocate required space*/
  !=   NULL )				/* and if we succeeded */
    if ( (arrowsp = rastack(spacesp,arrowsp,2,0,1,3)) /* space below arrow */
    ==   NULL ) goto end_of_job;	/* and quit if we failed */
/* --- init arrow subraster parameters --- */
arrowsp->size = size;			/*propagate base font size forward*/
arrowsp->baseline = height+vspace-1;	/* set baseline at bottom of arrow */
/* --- add limits above/below arrow, as necessary --- */
if ( subsp != NULL )			/* stack subscript below arrow */
  if ( (arrowsp = rastack(subsp,arrowsp,2,0,1,3)) /* subscript below arrow */
  ==   NULL ) goto end_of_job;		/* quit if failed */
if ( supsp != NULL )			/* stack superscript above arrow */
  if ( (arrowsp = rastack(arrowsp,supsp,1,vspace,1,3)) /*supsc above arrow*/
  ==   NULL ) goto end_of_job;		/* quit if failed */
/* --- return arrow (or NULL) to caller --- */
end_of_job:
  return ( arrowsp );
} /* --- end-of-function rastarrow() --- */


/* ==========================================================================
 * Function:	rastuparrow ( expression, size, basesp,  drctn, isBig, arg3 )
 * Purpose:	returns an up/down arrow subraster (e.g., for \longuparrow)
 * --------------------------------------------------------------------------
 * Arguments:	expression (I)	char **  to first char of null-terminated
 *				LaTeX expression (unused/unchanged)
 *		size (I)	int containing base font size (not used,
 *				just stored in subraster)
 *		basesp (I)	subraster *  to character (or subexpression)
 *				immediately preceding space, whose baseline
 *				and height params are transferred to space
 *		drctn (I)	int containing +1 for up, -1 for down,
 *				or 0 for updown
 *		isBig (I)	int containing 0 for ---> or 1 for ===>
 *		arg3 (I)	int unused
 * --------------------------------------------------------------------------
 * Returns:	( subraster * )	ptr to up/down arrow subraster
 *				or NULL for any error
 * --------------------------------------------------------------------------
 * Notes:     o	An optional argument [height] may *immediately* follow
 *		the \longxxx to explicitly set the arrow's height in pixels.
 *		For example, \longuparrow calculates a default height
 *		(as usual in LaTeX), whereas \longuparrow[25] explicitly
 *		draws a 25-pixel high arrow.  This can be used, e.g.,
 *		to draw commutative diagrams in conjunction with
 *		\array (and maybe with \stackrel and/or \relstack, too).
 *	      o	In case you really want to render, say, [f]---->[g], just
 *		use an intervening space, i.e., [f]\longuparrow~[g].
 *		In text use two spaces {\rm~[f]\longuparrow~~[g]}.
 * ======================================================================= */
/* --- entry point --- */
subraster *rastuparrow ( char **expression, int size, subraster *basesp,
			int drctn, int isBig, int arg3 )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
subraster *uparrow_subraster(), *arrowsp=NULL; /* subraster for arrow */
char	*texsubexpr(), heightarg[256];	/* parse for optional [height] */
char	*texscripts(), sub[1024],super[1024]; /* and _^limits after [width]*/
subraster *rasterize(), *subsp=NULL,*supsp=NULL; /*rasterize limits*/
subraster *rastcat();			/* cat superscript left, sub right */
int	evalterm();			/* evaluate [arg], {arg} */
int	height = 8 + 2*size,  width;	/* height, width for \longxxxarrow */
int	maxheight = 800;		/* max arrow height in pixels */
int	islimits = 1;			/*true to handle limits internally*/
int	limsize = size-1;		/* font size for limits */
int	pixsz = 1;			/*default #bits per pixel, 1=bitmap*/
/* -------------------------------------------------------------------------
construct blank subraster, and return it to caller
-------------------------------------------------------------------------- */
/* --- check for optional height arg and replace default height --- */
if ( *(*expression) == '[' )		/*check for []-enclosed optional arg*/
  { int heightval;			/* test height before using it */
    *expression = texsubexpr(*expression,heightarg,255,"[","]",0,0);
    heightval =				/* convert [height] to integer */
	(int)((unitlength*((double)evalterm(mimestore,heightarg)))+0.5);
    if ( heightval>=2 && heightval<=maxheight ) /* sanity check */
      height = heightval; }		/* replace deafault height */
/* --- now parse for limits, and bump expression past it(them) --- */
if ( islimits )				/* handling limits internally */
  { *expression = texscripts(*expression,sub,super,3); /* parse for limits */
    if ( *sub != '\000' )		/*have a subscript following arrow*/
      subsp = rasterize(sub,limsize);	/* so try to rasterize subscript */
    if ( *super != '\000' )		/*have superscript following arrow*/
      supsp = rasterize(super,limsize); } /*so try to rasterize superscript*/
/* --- set width based on height --- */
width = min2(17,max2(9,(height+2)/4));	/* width based on height */
width = 1 + (width/2)*2;		/* always force odd width */
/* --- generate arrow subraster --- */
if ( (arrowsp=uparrow_subraster(width,height,pixsz,drctn,isBig)) /*build arr*/
==   NULL ) goto end_of_job;		/* and quit if we failed */
/* --- init arrow subraster parameters --- */
arrowsp->size = size;			/*propagate base font size forward*/
arrowsp->baseline = height-1;		/* set baseline at bottom of arrow */
/* --- add limits above/below arrow, as necessary --- */
if ( supsp != NULL )			/* cat superscript to left of arrow*/
  { int	supht = (supsp->image)->height,	/* superscript height */
	deltab = (1+abs(height-supht))/2; /* baseline difference to center */
  supsp->baseline = supht-1;		/* force script baseline to bottom */
  if ( supht <= height )		/* arrow usually taller than script*/
	arrowsp->baseline -= deltab;	/* so bottom of script goes here */
  else	supsp->baseline -= deltab;	/* else bottom of arrow goes here */
  if ( (arrowsp = rastcat(supsp,arrowsp,3)) /* superscript left of arrow */
    ==   NULL ) goto end_of_job; }	/* quit if failed */
if ( subsp != NULL )			/* cat subscript to right of arrow */
  { int	subht = (subsp->image)->height,	/* subscript height */
	deltab = (1+abs(height-subht))/2; /* baseline difference to center */
  arrowsp->baseline = height-1;		/* reset arrow baseline to bottom */
  subsp->baseline = subht-1;		/* force script baseline to bottom */
  if ( subht <= height )		/* arrow usually taller than script*/
	arrowsp->baseline -= deltab;	/* so bottom of script goes here */
  else	subsp->baseline -= deltab;	/* else bottom of arrow goes here */
  if ( (arrowsp = rastcat(arrowsp,subsp,3)) /* subscript right of arrow */
    ==   NULL ) goto end_of_job; }	/* quit if failed */
/* --- return arrow (or NULL) to caller --- */
end_of_job:
  arrowsp->baseline = height-1;		/* reset arrow baseline to bottom */
  return ( arrowsp );
} /* --- end-of-function rastuparrow() --- */


/* ==========================================================================
 * Function:	rastoverlay (expression, size, basesp, overlay, offset2, arg3)
 * Purpose:	overlays one raster on another
 * --------------------------------------------------------------------------
 * Arguments:	expression (I/O) char **  to first char of null-terminated
 *				string immediately following overlay \cmd to
 *				be rasterized, and returning ptr immediately
 *				following last character processed.
 *		size (I)	int containing 0-7 default font size
 *		basesp (I)	subraster *  to character (or subexpression)
 *				immediately preceding overlay \cmd
 *				(unused, but passed for consistency)
 *		overlay (I)	int containing 1 to overlay / (e.g., \not)
 *				or NOVALUE to pick up 2nd arg from expression
 *		offset2 (I)	int containing #pixels to horizontally offset
 *				overlay relative to underlying symbol,
 *				positive(right) or negative or 0,
 *				or NOVALUE to pick up optional [offset] arg
 *		arg3 (I)	int unused
 * --------------------------------------------------------------------------
 * Returns:	( subraster * )	ptr to subraster corresponding to composite,
 *				or NULL for any parsing error
 * --------------------------------------------------------------------------
 * Notes:     o
 * ======================================================================= */
/* --- entry point --- */
subraster *rastoverlay ( char **expression, int size, subraster *basesp,
			int overlay, int offset2, int arg3 )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
char	*texsubexpr(),			/*parse expression for base,overlay*/
	expr1[512], expr2[512];		/* base, overlay */
subraster *rasterize(), *sp1=NULL, *sp2=NULL, /*rasterize 1=base, 2=overlay*/
	*new_subraster();		/*explicitly alloc sp2 if necessary*/
subraster *rastcompose(), *overlaysp=NULL; /*subraster for composite overlay*/
int	isalign = 0;			/* true to align baselines */
int	line_raster();			/* draw diagonal for \Not */
int	evalterm();			/* evaluate [arg], {arg} */
/* -------------------------------------------------------------------------
Obtain base, and maybe overlay, and rasterize them
-------------------------------------------------------------------------- */
/* --- check for optional offset2 arg  --- */
if ( offset2 == NOVALUE )		/* only if not explicitly specified*/
 if ( *(*expression) == '[' )		/*check for []-enclosed optional arg*/
  { int offsetval;			/* test before using it */
    *expression = texsubexpr(*expression,expr2,511,"[","]",0,0);
    offsetval =				/* convert [offset2] to int */
	(int)(((double)evalterm(mimestore,expr2))+0.5);
    if ( abs(offsetval) <= 25 )		/* sanity check */
      offset2 = offsetval; }		/* replace deafault */
if ( offset2 == NOVALUE ) offset2 = 0;	/* novalue means no offset */
/* --- parse for base, bump expression past it, and rasterize it --- */
*expression = texsubexpr(*expression,expr1,511,"{","}",0,0);
if ( isempty(expr1) ) goto end_of_job;	/* nothing to overlay, so quit */
rastlift1 = rastlift = 0;		/* reset all raisebox() lifts */
if ( strstr(expr1,"\\raise") != NULL )	/* have a \raisebox */
  isalign = 2;				/* so align baselines */
if ( (sp1=rasterize(expr1,size))	/* rasterize base expression */
==   NULL ) goto end_of_job;		/* quit if failed to rasterize */
overlaysp = sp1;			/*in case we return with no overlay*/
rastlift1 = rastlift;			/* save lift for base expression */
/* --- get overlay expression, and rasterize it --- */
if ( overlay == NOVALUE )		/* get overlay from input stream */
  { *expression = texsubexpr(*expression,expr2,511,"{","}",0,0);
    if ( !isempty(expr2) ) {		/* have an overlay */
      if ( strstr(expr2,"\\raise") != NULL ) /* have a \raisebox */
	isalign = 2;			/* so align baselines */
      sp2 = rasterize(expr2,size); } }	/* rasterize overlay expression */
else					/* use specific built-in overlay */
  switch ( overlay )
    {
    default: break;
    case 1:				/* e.g., \not overlays slash */
      sp2 = rasterize("/",size+1);	/* rasterize overlay expression */
      isalign = 0;			/* automatically handled correctly */
      offset2 = max2(1,size-3);		/* push / right a bit */
      offset2 = 0;
      break;
    case 2:				/* e.g., \Not draws diagonal */
      sp2 = NULL;			/* no overlay required */
      isalign = 0;			/* automatically handled correctly */
      if ( overlaysp != NULL )		/* check that we have raster */
	{ raster *rp = overlaysp->image; /* raster to be \Not-ed */
	  int width=rp->width, height=rp->height; /* raster dimensions */
	  if ( 0 )			/* diagonal within bounding box */
	   line_raster(rp,0,width-1,height-1,0,1); /* just draw diagonal */
	  else				/* construct "wide" diagonal */
	   { int margin=3;		/* desired extra margin width */
	     sp2 = new_subraster(width+margin,height+margin,1); /*alloc it*/
	     if ( sp2 != NULL )		/* allocated successfully */
	      line_raster(sp2->image,0,width+margin-1,height+margin-1,0,1);}}
      break;
    case 3:				/* e.g., \sout for strikeout */
      sp2 = NULL;			/* no overlay required */
      if ( overlaysp != NULL )		/* check that we have raster */
	{ raster *rp = overlaysp->image; /* raster to be \sout-ed */
	  int width=rp->width, height=rp->height; /* raster dimensions */
	  int baseline = (overlaysp->baseline)-rastlift; /*skip descenders*/
	  int midrow = max2(0,min2(height-1,offset2+((baseline+1)/2)));
	  if ( 1 )			/* strikeout within bounding box */
	    line_raster(rp,midrow,0,midrow,width-1,1); } /*draw strikeout*/
      break;
    } /* --- end-of-switch(overlay) --- */
if ( sp2 == NULL ) goto end_of_job;	/*return sp1 if failed to rasterize*/
/* -------------------------------------------------------------------------
construct composite overlay
-------------------------------------------------------------------------- */
overlaysp = rastcompose(sp1,sp2,offset2,isalign,3);
end_of_job:
  return ( overlaysp );
} /* --- end-of-function rastoverlay() --- */


/* ==========================================================================
 * Function:	rastfrac ( expression, size, basesp,  isfrac, arg2, arg3 )
 * Purpose:	\frac,\atop handler, returns a subraster corresponding to
 *		expression (immediately following \frac,\atop) at font size
 * --------------------------------------------------------------------------
 * Arguments:	expression (I/O) char **  to first char of null-terminated
 *				string immediately following \frac to be
 *				rasterized, and returning ptr immediately
 *				following last character processed.
 *		size (I)	int containing 0-7 default font size
 *		basesp (I)	subraster *  to character (or subexpression)
 *				immediately preceding \frac
 *				(unused, but passed for consistency)
 *		isfrac (I)	int containing true to draw horizontal line
 *				between numerator and denominator,
 *				or false not to draw it (for \atop).
 *		arg2 (I)	int unused
 *		arg3 (I)	int unused
 * --------------------------------------------------------------------------
 * Returns:	( subraster * )	ptr to subraster corresponding to fraction,
 *				or NULL for any parsing error
 * --------------------------------------------------------------------------
 * Notes:     o
 * ======================================================================= */
/* --- entry point --- */
subraster *rastfrac ( char **expression, int size, subraster *basesp,
			int isfrac, int arg2, int arg3 )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
char	*texsubexpr(),			/*parse expression for numer,denom*/
	numer[MAXSUBXSZ+1], denom[MAXSUBXSZ+1]; /* parsed numer, denom */
subraster *rasterize(), *numsp=NULL, *densp=NULL; /*rasterize numer, denom*/
subraster *rastack(), *fracsp=NULL;	/* subraster for numer/denom */
subraster *new_subraster()/*, *spacesp=NULL*/; /* space for num or den */
int	width=0,			/* width of constructed raster */
	numheight=0;			/* height of numerator */
int	baseht=0, baseln=0;		/* height,baseline of base symbol */
/*int	istweak = 1;*/			/*true to tweak baseline alignment*/
int	rule_raster(),			/* draw horizontal line for frac */
	lineheight = (isfrac?1:0);	/* thickness of fraction line */
int	vspace = (size>2?2:1);		/*vertical space between components*/
int	delete_subraster();		/*free work areas in case of error*/
int	type_raster();			/* display debugging output */
/* -------------------------------------------------------------------------
Obtain numerator and denominator, and rasterize them
-------------------------------------------------------------------------- */
/* --- first parse out optional \frac[] argument(s) --- */
if ( *(*expression) == '[' ) {		/* check for []-enclosed arg(s) */
  char optarg[512];			/* buffer for optional \frac[arg] */
  *expression = texsubexpr(*expression,optarg,511,"[","]",0,0);
  if ( !isempty(optarg) ) {		/* got optional arg */
    char *semi = strchr(optarg,';');	/* optionally followed by ; */
    if ( semi != NULL ) {		/* found further optional ; */
      char *comma = strchr(semi+1,',');	/* further optionally followed by , */
      *semi = '\000';			/* null-terminate optarg at ; */
      if ( comma != NULL ) *comma = '\000'; /* and that arg at (first) , */
      /* nextarg=atoi(semi+1)); */	/* convert argument after ; */
      isfrac = atoi(semi+1);		/* kludge for \frac[;0] = \atop */
      lineheight = (isfrac?1:0);	/* and reset lineheight */
      if ( comma != NULL ) {		/* have , and arg */
        /* nextarg=atoi(comma+1); */	/* convert argument after , */
        } /* --- end-of-if(comma!=NULL) --- */
      } /* --- end-of-if(semi!=NULL) --- */
    trimwhite(optarg);			/* remove leading/trailing whitespace*/
    if ( !isempty(optarg) )		/* wasn't just ";other,args" */
      vspace = atoi(optarg);		/* convert optarg to vspace int */
    if ( vspace<0 || vspace>999 )	/* sanity check (revert to default) */
      vspace = (size>2?2:1);		/* back to original default */
    } /* --- end-of-if(!isempty(optarg)) --- */
  } /* --- end-of-if(**expression=='[') --- */
/* ---then parse for numerator,denominator and bump expression past them--- */
*expression = texsubexpr(*expression,numer,0,"{","}",0,0);
*expression = texsubexpr(*expression,denom,0,"{","}",0,0);
if ( *numer=='\000' && *denom=='\000' )	/* missing both components of frac */
  goto end_of_job;			/* nothing to do, so quit */
/* --- rasterize numerator, denominator --- */
if ( *numer != '\000' )			/* have a numerator */
 if ( (numsp = rasterize(numer,size-1))	/* so rasterize numer at size-1 */
 ==   NULL ) goto end_of_job;		/* and quit if failed */
if ( *denom != '\000' )			/* have a denominator */
 if ( (densp = rasterize(denom,size-1))	/* so rasterize denom at size-1 */
 ==   NULL )				/* failed */
  { if ( numsp != NULL )		/* already rasterized numerator */
      delete_subraster(numsp);		/* so free now-unneeded numerator */
    goto end_of_job; }			/* and quit */
/* --- if one component missing, use a blank space for it --- */
if ( numsp == NULL )			/* no numerator given */
  numsp = rasterize("[?]",size-1);	/* missing numerator */
if ( densp == NULL )			/* no denominator given */
  densp = rasterize("[?]",size-1);	/* missing denominator */
/* --- check that we got both components --- */
if ( numsp==NULL || densp==NULL )	/* some problem */
  { delete_subraster(numsp);		/*delete numerator (if it existed)*/
    delete_subraster(densp);		/*delete denominator (if it existed)*/
    goto end_of_job; }			/* and quit */
/* --- get height of numerator (to determine where line belongs) --- */
numheight = (numsp->image)->height;	/* get numerator's height */
/* -------------------------------------------------------------------------
construct raster with numerator stacked over denominator
-------------------------------------------------------------------------- */
/* --- construct raster with numer/denom --- */
if ( (fracsp = rastack(densp,numsp,0,(2*vspace)+lineheight,1,3))/*numer/denom*/
==  NULL )				/* failed to construct numer/denom */
  { delete_subraster(numsp);		/* so free now-unneeded numerator */
    delete_subraster(densp);		/* and now-unneeded denominator */
    goto end_of_job; }			/* and then quit */
/* --- determine width of constructed raster --- */
width = (fracsp->image)->width;		/*just get width of embedded image*/
/* --- initialize subraster parameters --- */
fracsp->size = size;			/* propagate font size forward */
fracsp->baseline = (numheight+vspace+lineheight)+(size+2);/*default baseline*/
fracsp->type = FRACRASTER;		/* signal \frac image */
if ( basesp != (subraster *)NULL )	/* we have base symbol for frac */
  { baseht = (basesp->image)->height; 	/* height of base symbol */
    baseln =  basesp->baseline;		/* and its baseline */
  } /* --- end-of-if(basesp!=NULL) --- */
/* -------------------------------------------------------------------------
draw horizontal line between numerator and denominator
-------------------------------------------------------------------------- */
fraccenterline = numheight+vspace;	/* signal that we have a \frac */
if ( isfrac )				/*line for \frac, but not for \atop*/
  rule_raster(fracsp->image,fraccenterline,0,width,lineheight,0);
/* -------------------------------------------------------------------------
return final result to caller
-------------------------------------------------------------------------- */
end_of_job:
  if ( msgfp!=NULL && msglevel>=99 )
    { fprintf(msgfp,"rastfrac> returning %s\n",(fracsp==NULL?"null":"..."));
      if ( fracsp != NULL )		/* have a constructed raster */
	type_raster(fracsp->image,msgfp); } /* display constructed raster */
  return ( fracsp );
} /* --- end-of-function rastfrac() --- */


/* ==========================================================================
 * Function:	rastackrel ( expression, size, basesp,  base, arg2, arg3 )
 * Purpose:	\stackrel handler, returns a subraster corresponding to
 *		stacked relation
 * --------------------------------------------------------------------------
 * Arguments:	expression (I/O) char **  to first char of null-terminated
 *				string immediately following \stackrel to be
 *				rasterized, and returning ptr immediately
 *				following last character processed.
 *		size (I)	int containing 0-7 default font size
 *		basesp (I)	subraster *  to character (or subexpression)
 *				immediately preceding \stackrel
 *				(unused, but passed for consistency)
 *		base (I)	int containing 1 if upper/first subexpression
 *				is base relation, or 2 if lower/second is
 *		arg2 (I)	int unused
 *		arg3 (I)	int unused
 * --------------------------------------------------------------------------
 * Returns:	( subraster * )	ptr to subraster corresponding to stacked
 *				relation, or NULL for any parsing error
 * --------------------------------------------------------------------------
 * Notes:     o
 * ======================================================================= */
/* --- entry point --- */
subraster *rastackrel ( char **expression, int size, subraster *basesp,
			int base, int arg2, int arg3 )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
char	*texsubexpr(),			/*parse expression for upper,lower*/
	upper[MAXSUBXSZ+1], lower[MAXSUBXSZ+1];	/* parsed upper, lower */
subraster *rasterize(), *upsp=NULL, *lowsp=NULL; /* rasterize upper, lower */
subraster *rastack(), *relsp=NULL;	/* subraster for upper/lower */
int	upsize  = (base==1? size:size-1), /* font size for upper component */
	lowsize = (base==2? size:size-1); /* font size for lower component */
int	vspace = 1;			/*vertical space between components*/
int	delete_subraster();		/*free work areas in case of error*/
/* -------------------------------------------------------------------------
Obtain numerator and denominator, and rasterize them
-------------------------------------------------------------------------- */
/* --- parse for numerator,denominator and bump expression past them --- */
*expression = texsubexpr(*expression,upper,0,"{","}",0,0);
*expression = texsubexpr(*expression,lower,0,"{","}",0,0);
if ( *upper=='\000' || *lower=='\000' )	/* missing either component */
  goto end_of_job;			/* nothing to do, so quit */
/* --- rasterize upper, lower --- */
if ( *upper != '\000' )			/* have upper component */
 if ( (upsp = rasterize(upper,upsize))	/* so rasterize upper component */
 ==   NULL ) goto end_of_job;		/* and quit if failed */
if ( *lower != '\000' )			/* have lower component */
 if ( (lowsp = rasterize(lower,lowsize)) /* so rasterize lower component */
 ==   NULL )				/* failed */
  { if ( upsp != NULL )			/* already rasterized upper */
      delete_subraster(upsp);		/* so free now-unneeded upper */
    goto end_of_job; }			/* and quit */
/* -------------------------------------------------------------------------
construct stacked relation raster
-------------------------------------------------------------------------- */
/* --- construct stacked relation --- */
if ( (relsp = rastack(lowsp,upsp,3-base,vspace,1,3)) /* stacked relation */
==   NULL ) goto end_of_job;		/* quit if failed */
/* --- initialize subraster parameters --- */
relsp->size = size;			/* propagate font size forward */
/* -------------------------------------------------------------------------
return final result to caller
-------------------------------------------------------------------------- */
end_of_job:
  return ( relsp );
} /* --- end-of-function rastackrel() --- */


/* ==========================================================================
 * Function:	rastmathfunc ( expression, size, basesp,  base, arg2, arg3 )
 * Purpose:	\log, \lim, etc handler, returns a subraster corresponding
 *		to math functions
 * --------------------------------------------------------------------------
 * Arguments:	expression (I/O) char **  to first char of null-terminated
 *				string immediately following \mathfunc to be
 *				rasterized, and returning ptr immediately
 *				following last character processed.
 *		size (I)	int containing 0-7 default font size
 *		basesp (I)	subraster *  to character (or subexpression)
 *				immediately preceding \mathfunc
 *				(unused, but passed for consistency)
 *		mathfunc (I)	int containing 1=arccos, 2=arcsin, etc.
 *		islimits (I)	int containing 1 if function may have
 *				limits underneath, e.g., \lim_{n\to\infty}
 *		arg3 (I)	int unused
 * --------------------------------------------------------------------------
 * Returns:	( subraster * )	ptr to subraster corresponding to mathfunc,
 *				or NULL for any parsing error
 * --------------------------------------------------------------------------
 * Notes:     o
 * ======================================================================= */
/* --- entry point --- */
subraster *rastmathfunc ( char **expression, int size, subraster *basesp,
			int mathfunc, int islimits, int arg3 )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
char	*texscripts(),			/* parse expression for _limits */
	func[MAXTOKNSZ+1], limits[MAXSUBXSZ+1]; /*func as {\rm func}, limits*/
char	*texsubexpr(),			/* parse expression for arg */
	funcarg[MAXTOKNSZ+1];		/* optional func arg */
subraster *rasterize(), *funcsp=NULL, *limsp=NULL; /*rasterize func,limits*/
subraster *rastack(), *mathfuncsp=NULL;	/* subraster for mathfunc/limits */
int	limsize = size-1;		/* font size for limits */
int	vspace = 1;			/*vertical space between components*/
int	delete_subraster();		/*free work areas in case of error*/
/* --- table of function names by mathfunc number --- */
static	int  numnames = 34;		/* number of names in table */
static	char *funcnames[] = {
	"error",			/*  0 index is illegal/error bucket*/
	"arccos",  "arcsin",  "arctan",	/*  1 -  3 */
	"arg",     "cos",     "cosh",	/*  4 -  6 */
	"cot",     "coth",    "csc",	/*  7 -  9 */
	"deg",     "det",     "dim",	/* 10 - 12 */
	"exp",     "gcd",     "hom",	/* 13 - 15 */
	"inf",     "ker",     "lg",	/* 16 - 18 */
	"lim",     "liminf",  "limsup",	/* 19 - 21 */
	"ln",      "log",     "max",	/* 22 - 24 */
	"min",     "Pr",      "sec",	/* 25 - 27 */
	"sin",     "sinh",    "sup",	/* 28 - 30 */
	"tan",     "tanh",		/* 31 - 32 */
	/* --- extra mimetex funcnames --- */
	"tr",				/* 33 */
	"pmod"				/* 34 */
	} ;
/* -------------------------------------------------------------------------
set up and rasterize function name in \rm
-------------------------------------------------------------------------- */
if ( mathfunc<0 || mathfunc>numnames ) mathfunc=0; /* check index bounds */
switch ( mathfunc )			/* check for special processing */
  {
  default:				/* no special processing */
    strcpy(func,"{\\rm~");		/* init string with {\rm~ */
    strcat(func,funcnames[mathfunc]);	/* concat function name */
    strcat(func,"}");			/* and add terminating } */
    break;
  case 34:				/* \pmod{x} --> (mod x) */
    /* --- parse for \pmod{arg} argument --- */
    *expression = texsubexpr(*expression,funcarg,2047,"{","}",0,0);
    strcpy(func,"{\\({\\rm~mod}");	/* init with {\left({\rm~mod} */
    strcat(func,"\\hspace2");		/* concat space */
    strcat(func,funcarg);		/* and \pmodargument */
    strcat(func,"\\)}");		/* and add terminating \right)} */
    break;
  } /* --- end-of-switch(mathfunc) --- */
if ( (funcsp = rasterize(func,size))	/* rasterize function name */
==   NULL ) goto end_of_job;		/* and quit if failed */
mathfuncsp = funcsp;			/* just return funcsp if no limits */
if ( !islimits ) goto end_of_job;	/* treat any subscript normally */
/* -------------------------------------------------------------------------
Obtain limits, if permitted and if provided, and rasterize them
-------------------------------------------------------------------------- */
/* --- parse for subscript limits, and bump expression past it(them) --- */
*expression = texscripts(*expression,limits,limits,1);
if ( *limits=='\000') goto end_of_job;	/* no limits, nothing to do, quit */
/* --- rasterize limits --- */
if ( (limsp = rasterize(limits,limsize)) /* rasterize limits */
==   NULL ) goto end_of_job;		/* and quit if failed */
/* -------------------------------------------------------------------------
construct func atop limits
-------------------------------------------------------------------------- */
/* --- construct func atop limits --- */
if ( (mathfuncsp = rastack(limsp,funcsp,2,vspace,1,3)) /* func atop limits */
==   NULL ) goto end_of_job;		/* quit if failed */
/* --- initialize subraster parameters --- */
mathfuncsp->size = size;		/* propagate font size forward */
/* -------------------------------------------------------------------------
return final result to caller
-------------------------------------------------------------------------- */
end_of_job:
  return ( mathfuncsp );
} /* --- end-of-function rastmathfunc() --- */


/* ==========================================================================
 * Function:	rastsqrt ( expression, size, basesp,  arg1, arg2, arg3 )
 * Purpose:	\sqrt handler, returns a subraster corresponding to
 *		expression (immediately following \sqrt) at font size
 * --------------------------------------------------------------------------
 * Arguments:	expression (I/O) char **  to first char of null-terminated
 *				string immediately following \sqrt to be
 *				rasterized, and returning ptr immediately
 *				following last character processed.
 *		size (I)	int containing 0-7 default font size
 *		basesp (I)	subraster *  to character (or subexpression)
 *				immediately preceding \accent
 *				(unused, but passed for consistency)
 *		arg1 (I)	int unused
 *		arg2 (I)	int unused
 *		arg3 (I)	int unused
 * --------------------------------------------------------------------------
 * Returns:	( subraster * )	ptr to subraster corresponding to expression,
 *				or NULL for any parsing error
 *				(expression ptr unchanged if error occurs)
 * --------------------------------------------------------------------------
 * Notes:     o
 * ======================================================================= */
/* --- entry point --- */
subraster *rastsqrt ( char **expression, int size, subraster *basesp,
			int arg1, int arg2, int arg3 )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
char	*texsubexpr(), subexpr[MAXSUBXSZ+1], /*parse subexpr to be sqrt-ed*/
	rootarg[MAXSUBXSZ+1];		/* optional \sqrt[rootarg]{...} */
subraster *rasterize(), *subsp=NULL;	/* rasterize subexpr */
subraster *accent_subraster(), *sqrtsp=NULL, /* subraster with the sqrt */
	*new_subraster(), *rootsp=NULL;	/* optionally preceded by [rootarg]*/
int	sqrtheight=0, sqrtwidth=0, surdwidth=0,	/* height,width of sqrt */
	rootheight=0, rootwidth=0,	/* height,width of rootarg raster */
	subheight=0, subwidth=0, pixsz=0; /* height,width,pixsz of subexpr */
int	rastput();			/* put subexpr in constructed sqrt */
int	overspace = 2;			/*space between subexpr and overbar*/
int	delete_subraster();		/* free work areas */
/* -------------------------------------------------------------------------
Obtain subexpression to be sqrt-ed, and rasterize it
-------------------------------------------------------------------------- */
/* --- first check for optional \sqrt[rootarg]{...} --- */
if ( *(*expression) == '[' )		/*check for []-enclosed optional arg*/
  { *expression = texsubexpr(*expression,rootarg,0,"[","]",0,0);
    if ( *rootarg != '\000' )		/* got rootarg */
     if ( (rootsp=rasterize(rootarg,size-1)) /*rasterize it at smaller size*/
     != NULL )				/* rasterized successfully */
      {	rootheight = (rootsp->image)->height;  /* get height of rootarg */
	rootwidth  = (rootsp->image)->width; } /* and its width */
  } /* --- end-of-if(**expression=='[') --- */
/* --- parse for subexpr to be sqrt-ed, and bump expression past it --- */
*expression = texsubexpr(*expression,subexpr,0,"{","}",0,0);
if ( *subexpr == '\000' )		/* couldn't get subexpression */
  goto end_of_job;			/* nothing to do, so quit */
/* --- rasterize subexpression to be accented --- */
if ( (subsp = rasterize(subexpr,size))	/*rasterize subexpr at original size*/
==   NULL ) goto end_of_job;		/* quit if failed */
/* -------------------------------------------------------------------------
determine height and width of sqrt raster to be constructed
-------------------------------------------------------------------------- */
/* --- first get height and width of subexpr --- */
subheight = (subsp->image)->height;	/* height of subexpr */
subwidth  = (subsp->image)->width;	/* and its width */
pixsz     = (subsp->image)->pixsz;	/* pixsz remains constant */
/* --- determine height and width of sqrt to contain subexpr --- */
sqrtheight = subheight + overspace;	/* subexpr + blank line + overbar */
surdwidth  = SQRTWIDTH(sqrtheight,(rootheight<1?2:1)); /* width of surd */
sqrtwidth  = subwidth + surdwidth + 1;	/* total width */
/* -------------------------------------------------------------------------
construct sqrt (with room to move in subexpr) and embed subexpr in it
-------------------------------------------------------------------------- */
/* --- construct sqrt --- */
if ( (sqrtsp=accent_subraster(SQRTACCENT,
(rootheight<1?sqrtwidth:(-sqrtwidth)),sqrtheight,0,pixsz))
==   NULL ) goto end_of_job;		/* quit if failed to build sqrt */
/* --- embed subexpr in sqrt at lower-right corner--- */
rastput(sqrtsp->image,subsp->image,overspace,sqrtwidth-subwidth,1);
sqrtsp->baseline = subsp->baseline + overspace; /* adjust baseline */
/* --- "embed" rootarg at upper-left --- */
if ( rootsp != NULL )			/*have optional \sqrt[rootarg]{...}*/
  {
  /* --- allocate full raster to contain sqrtsp and rootsp --- */
  int fullwidth = sqrtwidth +rootwidth - min2(rootwidth,max2(0,surdwidth-4)),
      fullheight= sqrtheight+rootheight- min2(rootheight,3+size);
  subraster *fullsp = new_subraster(fullwidth,fullheight,pixsz);
  if ( fullsp != NULL )			/* allocated successfully */
    { /* --- embed sqrtsp exactly at lower-right corner --- */
      rastput(fullsp->image,sqrtsp->image, /* exactly at lower-right corner*/
	fullheight-sqrtheight,fullwidth-sqrtwidth,1);
      /* --- embed rootsp near upper-left, nestled above leading surd --- */
      rastput(fullsp->image,rootsp->image,
	0,max2(0,surdwidth-rootwidth-2-size),0);
      /* --- replace sqrtsp with fullsp --- */
      delete_subraster(sqrtsp);		/* free original sqrtsp */
      sqrtsp = fullsp;			/* and repoint it to fullsp instead*/
      sqrtsp->baseline = fullheight - (subheight - subsp->baseline); }
  } /* --- end-of-if(rootsp!=NULL) --- */
/* --- initialize subraster parameters --- */
sqrtsp->size = size;			/* propagate font size forward */
/* -------------------------------------------------------------------------
free unneeded component subrasters and return final result to caller
-------------------------------------------------------------------------- */
end_of_job:
  if ( subsp != NULL ) delete_subraster(subsp); /* free unneeded subexpr */
  return ( sqrtsp );
} /* --- end-of-function rastsqrt() --- */


/* ==========================================================================
 * Function:	rastaccent (expression,size,basesp,accent,isabove,isscript)
 * Purpose:	\hat, \vec, \etc handler, returns a subraster corresponding
 *		to expression (immediately following \accent) at font size
 * --------------------------------------------------------------------------
 * Arguments:	expression (I/O) char **  to first char of null-terminated
 *				string immediately following \accent to be
 *				rasterized, and returning ptr immediately
 *				following last character processed.
 *		size (I)	int containing 0-7 default font size
 *		basesp (I)	subraster *  to character (or subexpression)
 *				immediately preceding \accent
 *				(unused, but passed for consistency)
 *		accent (I)	int containing HATACCENT or VECACCENT, etc,
 *				between numerator and denominator,
 *				or false not to draw it (for \over).
 *		isabove (I)	int containing true if accent is above
 *				expression to be accented, or false
 *				if accent is below (e.g., underbrace)
 *		isscript (I)	int containing true if sub/superscripts
 *				allowed (for under/overbrace), or 0 if not.
 * --------------------------------------------------------------------------
 * Returns:	( subraster * )	ptr to subraster corresponding to expression,
 *				or NULL for any parsing error
 *				(expression ptr unchanged if error occurs)
 * --------------------------------------------------------------------------
 * Notes:     o	Also handles \overbrace{}^{} and \underbrace{}_{} by way
 *		of isabove and isscript args.
 * ======================================================================= */
/* --- entry point --- */
subraster *rastaccent ( char **expression, int size, subraster *basesp,
			int accent, int isabove, int isscript )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
char	*texsubexpr(), subexpr[MAXSUBXSZ+1]; /*parse subexpr to be accented*/
char	*texscripts(), *script=NULL,	/* \under,overbrace allow scripts */
	subscript[MAXTOKNSZ+1], supscript[MAXTOKNSZ+1];	/* parsed scripts */
subraster *rasterize(), *subsp=NULL, *scrsp=NULL; /*rasterize subexpr,script*/
subraster *rastack(), *accsubsp=NULL;	/* stack accent, subexpr, script */
subraster *accent_subraster(), *accsp=NULL; /*raster for the accent itself*/
int	accheight=0, accwidth=0, accdir=0,/*accent height, width, direction*/
	subheight=0, subwidth=0, pixsz=0; /* height,width,pixsz of subexpr */
int	delete_subraster();		/*free work areas in case of error*/
int	vspace = 0;			/*vertical space between accent,sub*/
/* -------------------------------------------------------------------------
Obtain subexpression to be accented, and rasterize it
-------------------------------------------------------------------------- */
/* --- parse for subexpr to be accented, and bump expression past it --- */
*expression = texsubexpr(*expression,subexpr,0,"{","}",0,0);
if ( *subexpr=='\000' )			/* couldn't get subexpression */
  goto end_of_job;			/* nothing to do, so quit */
/* --- rasterize subexpression to be accented --- */
if ( (subsp = rasterize(subexpr,size))	/*rasterize subexpr at original size*/
==   NULL ) goto end_of_job;		/* quit if failed */
/* -------------------------------------------------------------------------
determine desired accent width and height
-------------------------------------------------------------------------- */
/* --- first get height and width of subexpr --- */
subheight = (subsp->image)->height;	/* height of subexpr */
subwidth  = (subsp->image)->width;	/* and its width is overall width */
pixsz     = (subsp->image)->pixsz;	/* original pixsz remains constant */
/* --- determine desired width, height of accent --- */
accwidth = subwidth;			/* same width as subexpr */
accheight = 4;				/* default for bars */
switch ( accent )
  { default: break;			/* default okay */
  case DOTACCENT: case DDOTACCENT:
    accheight = (size<4? 3:4);		/* default for dots */
    break;
  case VECACCENT:
    vspace = 1;				/* set 1-pixel vertical space */
    accdir = isscript;			/* +1=right,-1=left,0=lr; +10for==>*/
    isscript = 0;			/* >>don't<< signal sub/supscript */
  case HATACCENT:
    accheight = 7;			/* default */
    if ( subwidth < 10 ) accheight = 5;	/* unless small width */
      else if ( subwidth > 25 ) accheight = 9; /* or large */
    break;
  } /* --- end-of-switch(accent) --- */
accheight = min2(accheight,subheight);	/*never higher than accented subexpr*/
/* -------------------------------------------------------------------------
construct accent, and construct subraster with accent over (or under) subexpr
-------------------------------------------------------------------------- */
/* --- first construct accent --- */
if ( (accsp = accent_subraster(accent,accwidth,accheight,accdir,pixsz))
==   NULL ) goto end_of_job;		/* quit if failed to build accent */
/* --- now stack accent above (or below) subexpr, and free both args --- */
accsubsp = (isabove? rastack(subsp,accsp,1,vspace,1,3)/*accent above subexpr*/
           : rastack(accsp,subsp,2,vspace,1,3));      /*accent below subexpr*/
if ( accsubsp == NULL )			/* failed to stack accent */
  { delete_subraster(subsp);		/* free unneeded subsp */
    delete_subraster(accsp);		/* and unneeded accsp */
    goto end_of_job; }			/* and quit */
/* -------------------------------------------------------------------------
look for super/subscript (annotation for over/underbrace)
-------------------------------------------------------------------------- */
/* --- first check whether accent permits accompanying annotations --- */
if ( !isscript ) goto end_of_job;	/* no annotations for this accent */
/* --- now get scripts if there actually are any --- */
*expression = texscripts(*expression,subscript,supscript,(isabove?2:1));
script = (isabove? supscript : subscript); /*select above^ or below_ script*/
if ( *script == '\000' ) goto end_of_job; /* no accompanying script */
/* --- rasterize script annotation at size-2 --- */
if ( (scrsp = rasterize(script,size-2)) /* rasterize script at size-2 */
==   NULL ) goto end_of_job;		/* quit if failed */
/* --- stack annotation above (or below) accent, and free both args --- */
accsubsp = (isabove? rastack(accsubsp,scrsp,1,0,1,3) /* accent above base */
           : rastack(scrsp,accsubsp,2,0,1,3));       /* accent below base */
/* -------------------------------------------------------------------------
return final result to caller
-------------------------------------------------------------------------- */
end_of_job:
  if ( accsubsp != NULL )		/* initialize subraster parameters */
    accsubsp->size = size;		/* propagate font size forward */
  return ( accsubsp );
} /* --- end-of-function rastaccent() --- */


/* ==========================================================================
 * Function:	rastfont (expression,size,basesp,ifontnum,arg2,arg3)
 * Purpose:	\cal{}, \scr{}, \etc handler, returns subraster corresponding
 *		to char(s) within {}'s rendered at size
 * --------------------------------------------------------------------------
 * Arguments:	expression (I/O) char **  to first char of null-terminated
 *				string immediately following \font to be
 *				rasterized, and returning ptr immediately
 *				following last character processed.
 *		size (I)	int containing 0-7 default font size
 *		basesp (I)	subraster *  to character (or subexpression)
 *				immediately preceding \accent
 *				(unused, but passed for consistency)
 *		ifontnum (I)	int containing 1 for \cal{}, 2 for \scr{}
 *		arg2 (I)	int unused
 *		arg3 (I)	int unused
 * --------------------------------------------------------------------------
 * Returns:	( subraster * )	ptr to subraster corresponding to chars
 *				between {}'s, or NULL for any parsing error
 * --------------------------------------------------------------------------
 * Notes:     o
 * ======================================================================= */
/* --- entry point --- */
subraster *rastfont ( char **expression, int size, subraster *basesp,
			int ifontnum, int arg2, int arg3 )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
char	*texsubexpr(), fontchars[MAXSUBXSZ+1], /* chars to render in font */
	subexpr[MAXSUBXSZ+1];		/* turn \cal{AB} into \calA\calB */
char	*pfchars=fontchars, fchar='\0';	/* run thru fontchars one at a time*/
char	*name = NULL;			/* fontinfo[ifontnum].name */
int	family = 0,			/* fontinfo[ifontnum].family */
	istext = 0,			/* fontinfo[ifontnum].istext */
	class = 0;			/* fontinfo[ifontnum].class */
subraster *rasterize(), *fontsp=NULL,	/* rasterize chars in font */
	*rastflags();			/* or just set flag to switch font */
int	oldsmashmargin = smashmargin;	/* turn off smash in text mode */
#if 0
/* --- fonts recognized by rastfont --- */
static	int  nfonts = 11;		/* legal font #'s are 1...nfonts */
static	struct {char *name; int class;}
  fonts[] =
    { /* --- name  class 1=upper,2=alpha,3=alnum,4=lower,5=digit,9=all --- */
	{ "\\math",	0 },
	{ "\\mathcal",	1 },		/*(1) calligraphic, uppercase */
	{ "\\mathscr",	1 },		/*(2) rsfs/script, uppercase */
	{ "\\textrm",	-1 },		/*(3) \rm,\text{abc} --> {\rm~abc} */
	{ "\\textit",	-1 },		/*(4) \it,\textit{abc}-->{\it~abc} */
	{ "\\mathbb",	-1 },		/*(5) \bb,\mathbb{abc}-->{\bb~abc} */
	{ "\\mathbf",	-1 },		/*(6) \bf,\mathbf{abc}-->{\bf~abc} */
	{ "\\mathrm",   -1 },		/*(7) \mathrm */
	{ "\\cyr",      -1 },		/*(8) \cyr */
	{ "\\textgreek",-1 },		/*(9) \textgreek */
	{ "\\textbfgreek",CMMI10BGR,1,-1 },/*(10) \textbfgreek{ab} */
	{ "\\textbbgreek",BBOLD10GR,1,-1 },/*(11) \textbbgreek{ab} */
	{ NULL,		0 }
    } ; /* --- end-of-fonts[] --- */
#endif
/* -------------------------------------------------------------------------
first get font name and class to determine type of conversion desired
-------------------------------------------------------------------------- */
if (ifontnum<=0 || ifontnum>nfontinfo) ifontnum=0; /*math if out-of-bounds*/
name   = fontinfo[ifontnum].name;	/* font name */
family = fontinfo[ifontnum].family;	/* font family */
istext = fontinfo[ifontnum].istext;	/*true in text mode (respect space)*/
class  = fontinfo[ifontnum].class;	/* font class */
if ( istext )				/* text (respect blanks) */
 { mathsmashmargin = smashmargin;	/* needed for \text{if $n-m$ even} */
   smashmargin = 0; }			/* don't smash internal blanks */
/* -------------------------------------------------------------------------
now convert \font{abc} --> {\font~abc}, or convert ABC to \calA\calB\calC
-------------------------------------------------------------------------- */
if ( 1 || class<0 )			/* not character-by-character */
 { 
 /* ---
 if \font not immediately followed by { then it has no arg, so just set flag
 ------------------------------------------------------------------------ */
 if ( *(*expression) != '{' )		/* no \font arg, so just set flag */
    {
    if ( msgfp!=NULL && msglevel>=99 )
     fprintf(msgfp,"rastfont> \\%s rastflags() for font#%d\n",name,ifontnum);
    fontsp = rastflags(expression,size,basesp,ISFONTFAM,ifontnum,arg3);
    goto end_of_job;
    } /* --- end-of-if(*(*expression)!='{') --- */
 /* ---
 convert \font{abc} --> {\font~abc}
 ---------------------------------- */
 /* --- parse for {fontchars} arg, and bump expression past it --- */
 *expression = texsubexpr(*expression,fontchars,0,"{","}",0,0);
 if ( msgfp!=NULL && msglevel>=99 )
  fprintf(msgfp,"rastfont> \\%s fontchars=\"%s\"\n",name,fontchars);
 /* --- convert all fontchars at the same time --- */
 strcpy(subexpr,"{");			/* start off with opening { */
 strcat(subexpr,name);			/* followed by font name */
 strcat(subexpr,"~");			/* followed by whitespace */
 strcat(subexpr,fontchars);		/* followed by all the chars */
 strcat(subexpr,"}");			/* terminate with closing } */
 } /* --- end-of-if(class<0) --- */
else					/* character-by-character */
 {
 /* ---
 convert ABC to \calA\calB\calC
 ------------------------------ */
 int	isprevchar=0;			/* true if prev char converted */
 /* --- parse for {fontchars} arg, and bump expression past it --- */
 *expression = texsubexpr(*expression,fontchars,0,"{","}",0,0);
 if ( msgfp!=NULL && msglevel>=99 )
  fprintf(msgfp,"rastfont> \\%s fontchars=\"%s\"\n",name,fontchars);
 /* --- convert fontchars one at a time --- */
 strcpy(subexpr,"{\\rm~");		/* start off with opening {\rm */
 strcpy(subexpr,"{");			/* nope, just start off with { */
 for ( pfchars=fontchars; (fchar= *pfchars)!='\000'; pfchars++ )
  {
  if ( isthischar(fchar,WHITEMATH) )	/* some whitespace */
    { if ( 0 || istext )		/* and we're in a text mode font */
	strcat(subexpr,"\\;"); }	/* so respect whitespace */
  else					/* char to be displayed in font */
    { int exprlen = 0;			/* #chars in subexpr before fchar */
      int isinclass = 0;		/* set true if fchar in font class */
      /* --- class: 1=upper, 2=alpha, 3=alnum, 4=lower, 5=digit, 9=all --- */
      switch ( class )			/* check if fchar is in font class */
	{ default: break;		/* no chars in unrecognized class */
	  case 1: if ( isupper((int)fchar) ) isinclass=1; break;
	  case 2: if ( isalpha((int)fchar) ) isinclass=1; break;
	  case 3: if ( isalnum((int)fchar) ) isinclass=1; break;
	  case 4: if ( islower((int)fchar) ) isinclass=1; break;
	  case 5: if ( isdigit((int)fchar) ) isinclass=1; break;
	  case 9: isinclass=1; break; }
      if ( isinclass )			/* convert current char to \font */
	{ strcat(subexpr,name);		/* by prefixing it with font name */
	  isprevchar = 1; }		/* and set flag to signal separator*/
      else				/* current char not in \font */
	{ if ( isprevchar )		/* extra separator only after \font*/
	   if ( isalpha(fchar) )	/* separator only before alpha */
	    strcat(subexpr,"~");	/* need separator after \font */
	  isprevchar = 0; }		/* reset flag for next char */
      exprlen = strlen(subexpr);	/* #chars so far */
      subexpr[exprlen] = fchar;		/*fchar immediately after \fontname*/
      subexpr[exprlen+1] = '\000'; }	/* replace terminating '\0' */
  } /* --- end-of-for(pfchars) --- */
 strcat(subexpr,"}");			/* add closing } */
 } /* --- end-of-if/else(class<0) --- */
/* -------------------------------------------------------------------------
rasterize subexpression containing chars to be rendered at font
-------------------------------------------------------------------------- */
if ( msgfp!=NULL && msglevel>=99 )
  fprintf(msgfp,"rastfont> subexpr=\"%s\"\n",subexpr);
if ( (fontsp = rasterize(subexpr,size))	/* rasterize chars in font */
==   NULL ) goto end_of_job;		/* and quit if failed */
/* -------------------------------------------------------------------------
back to caller with chars rendered in font
-------------------------------------------------------------------------- */
end_of_job:
  smashmargin = oldsmashmargin;		/* restore smash */
  mathsmashmargin = SMASHMARGIN;	/* this one probably not necessary */
  if ( istext && fontsp!=NULL )		/* raster contains text mode font */
    fontsp->type = blanksignal;		/* signal nosmash */
  return ( fontsp );			/* chars rendered in font */
} /* --- end-of-function rastfont() --- */


/* ==========================================================================
 * Function:	rastbegin ( expression, size, basesp, arg1, arg2, arg3 )
 * Purpose:	\begin{}...\end{}  handler, returns a subraster corresponding
 *		to array expression within environment, i.e., rewrites
 *		\begin{}...\end{} as mimeTeX equivalent, and rasterizes that.
 * --------------------------------------------------------------------------
 * Arguments:	expression (I/O) char **  to first char of null-terminated
 *				string immediately following \begin to be
 *				rasterized, and returning ptr immediately
 *				following last character processed.
 *		size (I)	int containing 0-7 default font size
 *		basesp (I)	subraster *  to character (or subexpression)
 *				immediately preceding \begin
 *				(unused, but passed for consistency)
 *		arg1 (I)	int unused
 *		arg2 (I)	int unused
 *		arg3 (I)	int unused
 * --------------------------------------------------------------------------
 * Returns:	( subraster * )	ptr to subraster corresponding to array
 *				expression, or NULL for any parsing error
 * --------------------------------------------------------------------------
 * Notes:     o
 * ======================================================================= */
/* --- entry point --- */
subraster *rastbegin ( char **expression, int size, subraster *basesp,
			int arg1, int arg2, int arg3 )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
char	*texsubexpr(), subexpr[MAXSUBXSZ+1], /* \begin{} environment params*/
	*exprptr=NULL,*begptr=NULL,*endptr=NULL,*braceptr=NULL; /* ptrs */
char	*begtoken="\\begin{", *endtoken="\\end{"; /*tokens we're looking for*/
int	strreplace();			/* replace substring in string */
char	*strchange();			/*\begin...\end --> {\begin...\end}*/
char	*delims = (char *)NULL;		/* mdelims[ienviron] */
subraster *rasterize(), *sp=NULL;	/* rasterize environment */
int	ienviron = 0;			/* environs[] index */
int	nbegins = 0;			/* #\begins nested beneath this one*/
int	envlen=0, sublen=0;		/* #chars in environ, subexpr */
static	int blevel = 0;			/* \begin...\end nesting level */
static	char *mdelims[] = { NULL, NULL, NULL, NULL,
	"()","[]","{}","||","==",	/* for pbBvVmatrix */
	NULL, NULL, NULL, NULL, "{.", NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
static	char *environs[] = {		/* types of environments we process*/
	"eqnarray",			/* 0 eqnarray environment */
	"array",			/* 1 array environment */
	"matrix",			/* 2 array environment */
	"tabular",			/* 3 array environment */
	"pmatrix",			/* 4 ( ) */
	"bmatrix",			/* 5 [ ] */
	"Bmatrix",			/* 6 { } */
	"vmatrix",			/* 7 | | */
	"Vmatrix",			/* 8 || || */
	"gather",			/* 9 gather environment */
	"align",			/* 10 align environment */
	"verbatim",			/* 11 verbatim environment */
	"picture",			/* 12 picture environment */
	"cases",			/* 13 cases environment */
	"equation",			/* 14 for \begin{equation} */
	NULL };				/* trailer */
/* -------------------------------------------------------------------------
determine type of environment we're beginning
-------------------------------------------------------------------------- */
/* --- first bump nesting level --- */
blevel++;				/* count \begin...\begin...'s */
/* --- \begin must be followed by {type_of_environment} --- */
exprptr = texsubexpr(*expression,subexpr,0,"{","}",0,0);
if ( *subexpr == '\000' ) goto end_of_job; /* no environment given */
while ( (delims=strchr(subexpr,'*')) != NULL ) /* have environment* */
  {strsqueeze(delims,1);}		/* treat it as environment */
/* --- look up environment in our table --- */
for ( ienviron=0; ;ienviron++ )		/* search table till NULL */
  if ( environs[ienviron] == NULL )	/* found NULL before match */
    goto end_of_job;			/* so quit */
  else					/* see if we have an exact match */
    if ( memcmp(environs[ienviron],subexpr,strlen(subexpr)) == 0 ) /*match*/
      break;				/* leave loop with ienviron index */
/* --- accumulate any additional params for this environment --- */
*subexpr = '\000';			/* reset subexpr to empty string */
delims = mdelims[ienviron];		/* mdelims[] string for ienviron */
if ( delims != NULL )			/* add appropriate opening delim */
  { strcpy(subexpr,"\\");		/* start with \ for (,[,{,|,= */
    strcat(subexpr,delims);		/* then add opening delim */
    subexpr[2] = '\000'; }		/* remove extraneous closing delim */
switch ( ienviron )
  {
  default: goto end_of_job;		/* environ not implemented yet */
  case 0:				/* \begin{eqnarray} */
    strcpy(subexpr,"\\array{rcl$");	/* set default rcl for eqnarray */
    break;
  case 1:  case 2:  case 3:		/* \begin{array} followed by {lcr} */
    strcpy(subexpr,"\\array{");		/*start with mimeTeX \array{ command*/
    skipwhite(exprptr);			/* bump to next non-white char */
    if ( *exprptr == '{' )		/* assume we have {lcr} argument */
      {	exprptr = texsubexpr(exprptr,subexpr+7,0,"{","}",0,0); /*add on lcr*/
	if ( *(subexpr+7) == '\000' ) goto end_of_job; /* quit if no lcr */
	strcat(subexpr,"$"); }		/* add terminating $ to lcr */
    break;
  case 4:  case 5:  case 6:		/* \begin{pmatrix} or b,B,v,Vmatrix */
  case 7:  case 8:
    strcat(subexpr,"\\array{");		/*start with mimeTeX \array{ command*/
    break;
  case 9:				/* gather */
    strcat(subexpr,"\\array{c$");	/* center equations */
    break;
  case 10:				/* align */
    strcat(subexpr,"\\array{rclrclrclrclrclrcl$"); /* a&=b & c&=d & etc */
    break;
  case 11:				/* verbatim */
    strcat(subexpr,"{\\rm ");		/* {\rm ...} */
    /*strcat(subexpr,"\\\\{\\rm ");*/	/* \\{\rm } doesn't work in context */
    break;
  case 12:				/* picture */
    strcat(subexpr,"\\picture");	/* picture environment */
    skipwhite(exprptr);			/* bump to next non-white char */
    if ( *exprptr == '(' )		/*assume we have (width,height) arg*/
      {	exprptr = texsubexpr(exprptr,subexpr+8,0,"(",")",0,1); /*add on arg*/
	if ( *(subexpr+8) == '\000' ) goto end_of_job; } /* quit if no arg */
    strcat(subexpr,"{");		/* opening {  after (width,height) */
    break;
  case 13:				/* cases */
    strcat(subexpr,"\\array{ll$");	/* a&b \\ c&d etc */
    break;
  case 14:				/* \begin{equation} */
    strcat(subexpr,"{");		/* just enclose expression in {}'s */
    break;
  } /* --- end-of-switch(ienviron) --- */
/* -------------------------------------------------------------------------
locate matching \end{...}
-------------------------------------------------------------------------- */
/* --- first \end following \begin --- */
if ( (endptr=strstr(exprptr,endtoken))	/* find 1st \end following \begin */
==   NULL ) goto end_of_job;		/* and quit if no \end found */
/* --- find matching endptr by pushing past any nested \begin's --- */
begptr = exprptr;			/* start after first \begin{...} */
while ( 1 )				/*break when we find matching \end*/
  {
  /* --- first, set ptr to closing } terminating current \end{...} --- */
  if ( (braceptr=strchr(endptr+1,'}'))	/* find 1st } following \end{ */
  ==   NULL ) goto end_of_job;		/* and quit if no } found */
  /* -- locate next nested \begin --- */
  if ( (begptr=strstr(begptr,begtoken))	/* find next \begin{...} */
  ==   NULL ) break;			/*no more, so we have matching \end*/
  begptr += strlen(begtoken);		/* push ptr past token */
  if ( begptr >= endptr ) break;	/* past endptr, so not nested */
  /* --- have nested \begin, so push forward to next \end --- */
  nbegins++;				/* count another nested \begin */
  if ( (endptr=strstr(endptr+strlen(endtoken),endtoken)) /* find next \end */
  ==   NULL ) goto end_of_job;		/* and quit if none found */
  } /* --- end-of-while(1) --- */
/* --- push expression past closing } of \end{} --- */
*expression = braceptr+1;		/* resume processing after } */
/* -------------------------------------------------------------------------
add on everything (i.e., the ...'s) between \begin{}[{}] ... \end{}
-------------------------------------------------------------------------- */
/* --- add on everything, completing subexpr for \begin{}...\end{} --- */
sublen = strlen(subexpr);		/* #chars in "preamble" */
envlen = (int)(endptr-exprptr);		/* #chars between \begin{}{}...\end */
memcpy(subexpr+sublen,exprptr,envlen);	/*concatanate environ after subexpr*/
subexpr[sublen+envlen] = '\000';	/* and null-terminate */
if ( 2 > 1 )				/* always... */
  strcat(subexpr,"}");			/* ...followed by terminating } */
/* --- add terminating \right), etc, if necessary --- */
if ( delims != (char *)NULL )		/* need closing delim */
 { strcat(subexpr,"\\");		/* start with \ for ),],},|,= */
   strcat(subexpr,delims+1); }		/* add appropriate closing delim */
/* -------------------------------------------------------------------------
change nested \begin...\end to {\begin...\end} so \array{} can handle them
-------------------------------------------------------------------------- */
if ( nbegins > 0 )			/* have nested begins */
 if ( blevel < 2 )			/* only need to do this once */
  {
  begptr = subexpr;			/* start at beginning of subexpr */
  while( (begptr=strstr(begptr,begtoken)) != NULL ) /* have \begin{...} */
    { strchange(0,begptr,"{");		/* \begin --> {\begin */
      begptr += strlen(begtoken); }	/* continue past {\begin */
  endptr = subexpr;			/* start at beginning of subexpr */
  while( (endptr=strstr(endptr,endtoken)) != NULL ) /* have \end{...} */
    if ( (braceptr=strchr(endptr+1,'}')) /* find 1st } following \end{ */
    ==   NULL ) goto end_of_job;	/* and quit if no } found */
    else				/* found terminating } */
     { strchange(0,braceptr,"}");	/* \end{...} --> \end{...}} */
       endptr = braceptr+1; }		/* continue past \end{...} */
  } /* --- end-of-if(nbegins>0) --- */
/* -------------------------------------------------------------------------
post process as necessary
-------------------------------------------------------------------------- */
switch ( ienviron )
  {
  default: break;			/* no post-processing required */
  case 10:				/* align */
    strreplace(subexpr,"&=","#*@*#=",0); /* tag all &='s */
    strreplace(subexpr,"&<","#*@*#<",0); /* tag all &<'s */
    strreplace(subexpr,"&\\lt","#*@*#<",0); /* tag all &\lt's */
    strreplace(subexpr,"&\\leq","#*@*#\\leq",0); /* tag all &\leq's */
    strreplace(subexpr,"&>","#*@*#>",0); /* tag all &>'s */
    strreplace(subexpr,"&\\gt","#*@*#>",0); /* tag all &\gt's */
    strreplace(subexpr,"&\\geq","#*@*#\\geq",0); /* tag all &\geq's */
    if ( nbegins < 1 )			/* don't modify nested arrays */
      strreplace(subexpr,"&","\\hspace{10}&\\hspace{10}",0); /* add space */
    strreplace(subexpr,"#*@*#=","& = &",0); /*restore and xlate tagged &='s*/
    strreplace(subexpr,"#*@*#<","& \\lt &",0); /*restore, xlate tagged &<'s*/
    strreplace(subexpr,"#*@*#\\leq","& \\leq &",0); /*xlate tagged &\leq's*/
    strreplace(subexpr,"#*@*#>","& \\gt &",0); /*restore, xlate tagged &>'s*/
    strreplace(subexpr,"#*@*#\\geq","& \\geq &",0); /*xlate tagged &\geq's*/
    break;
  case 11:				/* verbatim */
    strreplace(subexpr,"\n","\\\\",0);	/* xlate \n newline to latex \\ */
    /*strcat(subexpr,"\\\\");*/		/* add final latex \\ newline */
    break;
  case 12:				/* picture */
    strreplace(subexpr,"\\put "," ",0);	/*remove \put's (not really needed)*/
    strreplace(subexpr,"\\put(","(",0);	/*remove \put's (not really needed)*/
    strreplace(subexpr,"\\oval","\\circle",0); /* actually an ellipse */
    break;
  } /* --- end-of-switch(ienviron) --- */
/* -------------------------------------------------------------------------
return rasterized mimeTeX equivalent of \begin{}...\end{} environment
-------------------------------------------------------------------------- */
/* --- debugging output --- */
if ( msgfp!=NULL && msglevel>=99 )
  fprintf(msgfp,"rastbegin> subexpr=%s\n",subexpr);
/* --- rasterize mimeTeX equivalent of \begin{}...\end{} environment --- */
sp = rasterize(subexpr,size);		/* rasterize subexpr */
end_of_job:
  blevel--;				/* decrement \begin nesting level */
  return ( sp );			/* back to caller with sp or NULL */
} /* --- end-of-function rastbegin() --- */


/* ==========================================================================
 * Function:	rastarray ( expression, size, basesp, arg1, arg2, arg3 )
 * Purpose:	\array handler, returns a subraster corresponding to array
 *		expression (immediately following \array) at font size
 * --------------------------------------------------------------------------
 * Arguments:	expression (I/O) char **  to first char of null-terminated
 *				string immediately following \array to be
 *				rasterized, and returning ptr immediately
 *				following last character processed.
 *		size (I)	int containing 0-7 default font size
 *		basesp (I)	subraster *  to character (or subexpression)
 *				immediately preceding \array
 *				(unused, but passed for consistency)
 *		arg1 (I)	int unused
 *		arg2 (I)	int unused
 *		arg3 (I)	int unused
 * --------------------------------------------------------------------------
 * Returns:	( subraster * )	ptr to subraster corresponding to array
 *				expression, or NULL for any parsing error
 * --------------------------------------------------------------------------
 * Notes:     o	Summary of syntax...
 *			\array{3,lcrBC$a&b&c\\d&e&f\\etc}
 *	      o	The 3,lcrBC$ part is an optional "preamble".  The lcr means
 *		what you think, i.e., "horizontal" left,center,right
 *		justification down corresponding column.  The new BC means
 *		"vertical" baseline,center justification across corresponding
 *		row.  The leading 3 specifies the font size 0-4 to be used.
 *		You may also specify +1,-1,+2,-2, etc, which is used as an
 *		increment to the current font size, e.g., -1,lcr$ uses
 *		one font size smaller than current.  Without a leading
 *		+ or -, the font size is "absolute".
 *	      o	The preamble can also be just lcrBC$ without a leading
 *		size-part, or just 3$ without a trailing lcrBC-part.
 *		The default size is whatever is current, and the
 *		default justification is c(entered) and B(aseline).
 * ======================================================================= */
/* --- entry point --- */
subraster *rastarray ( char **expression, int size, subraster *basesp,
			int arg1, int arg2, int arg3 )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
char	*texsubexpr(), subexpr[MAXSUBXSZ+1], *exprptr, /*parse array subexpr*/
	 subtok[MAXTOKNSZ+1], *subptr=subtok, /* &,\\ inside { } not a delim*/
	 token[MAXTOKNSZ+1],  *tokptr=token, /* subexpr token to rasterize */
	*preamble(),   *preptr=token;	/*process optional size,lcr preamble*/
char	*coldelim="&", *rowdelim="\\";	/* need escaped rowdelim */
int	maxarraysz = 63;		/* max #rows, cols */
int	justify[65]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* -1,0,+1 = l,c,r */
	               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	  hline[65]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* hline above row? */
	               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	  vline[65]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /*vline left of col?*/
	               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
       colwidth[65]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /*widest tokn in col*/
	               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
      rowheight[65]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* "highest" in row */
	               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
     fixcolsize[65]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /*1=fixed col width*/
	               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
     fixrowsize[65]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /*1=fixed row height*/
	               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
      rowbaseln[65]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* baseline for row */
	               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
      vrowspace[65]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /*extra //[len]space*/
	               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
      rowcenter[65]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /*true = vcenter row*/
	               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static int /* --- propagate global values across arrays --- */
       gjustify[65]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* -1,0,+1 = l,c,r */
	               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
      gcolwidth[65]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /*widest tokn in col*/
	               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
     growheight[65]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* "highest" in row */
	               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    gfixcolsize[65]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /*1=fixed col width*/
	               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    gfixrowsize[65]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /*1=fixed row height*/
	               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
     growcenter[65]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /*true = vcenter row*/
	               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int	rowglobal=0, colglobal=0,	/* true to set global values */
	rowpropagate=0, colpropagate=0;	/* true if propagating values */
int	irow,nrows=0, icol,ncols[65],	/*#rows in array, #cols in each row*/
	maxcols=0;			/* max# cols in any single row */
int	itoken, ntokens=0,		/* index, total #tokens in array */
	subtoklen=0,			/* strlen of {...} subtoken */
	istokwhite=1,			/* true if token all whitespace */
	nnonwhite=0;			/* #non-white tokens */
int	isescape=0,wasescape=0,		/* current,prev chars escape? */
	ischarescaped=0,		/* is current char escaped? */
	nescapes=0;			/* #consecutive escapes */
subraster *rasterize(), *toksp[1025],	/* rasterize tokens */
	*new_subraster(), *arraysp=NULL; /* subraster for entire array */
raster	*arrayrp=NULL;			/* raster for entire array */
int	delete_subraster();		/* free toksp[] workspace at eoj */
int	rowspace=2, colspace=4,		/* blank space between rows, cols */
	hspace=1, vspace=1;		/*space to accommodate hline,vline*/
int	width=0, height=0,		/* width,height of array */
	leftcol=0, toprow=0;		/*upper-left corner for cell in it*/
int	rastput();			/* embed tokens/cells in array */
int	rule_raster();			/* draw hlines and vlines in array */
char	*hlchar="\\hline", *hdchar="\\hdash"; /* token signals hline */
char	*texchar(), hltoken[1025];	/* extract \hline from token */
int	ishonly=0, hltoklen, minhltoklen=3; /*flag, token must be \hl or \hd*/
int	isnewrow=1;			/* true for new row */
int	pixsz = 1;			/*default #bits per pixel, 1=bitmap*/
int	evalterm(), evalue=0;		/* evaluate [arg], {arg} */
static	int mydaemonlevel = 0;		/* check against global daemonlevel*/
/* -------------------------------------------------------------------------
Macros to determine extra raster space required for vline/hline
-------------------------------------------------------------------------- */
#define	vlinespace(icol) \
	( vline[icol] == 0?  0 :	/* no vline so no space needed */   \
	  ( icol<1 || icol>=maxcols? vspace+(colspace+1)/2 : vspace ) )
#define	hlinespace(irow) \
	( hline[irow] == 0?  0 :	/* no hline so no space needed */   \
	  ( irow<1 || irow>=nrows? hspace+(rowspace+1)/2 : hspace ) )
/* -------------------------------------------------------------------------
Obtain array subexpression
-------------------------------------------------------------------------- */
/* --- parse for array subexpression, and bump expression past it --- */
subexpr[1] = *subexpr = ' ';		/* set two leading blanks */
*expression = texsubexpr(*expression,subexpr+2,0,"{","}",0,0);
if ( msglevel>=29 && msgfp!=NULL )	/* debugging, display array */
  fprintf(msgfp,"rastarray> %.256s\n",subexpr+2);
if ( *(subexpr+2)=='\000' )		/* couldn't get subexpression */
  goto end_of_job;			/* nothing to do, so quit */
/* -------------------------------------------------------------------------
reset static arrays if main re-entered as daemon (or dll)
-------------------------------------------------------------------------- */
if ( mydaemonlevel != daemonlevel ) {	/* main re-entered */
  for ( icol=0; icol<=maxarraysz; icol++ ) /* for each array[] index */
    gjustify[icol]    = gcolwidth[icol]   = growheight[icol] =
    gfixcolsize[icol] = gfixrowsize[icol] = growcenter[icol] = 0;
  mydaemonlevel = daemonlevel; }	/* update mydaemonlevel */
/* -------------------------------------------------------------------------
process optional size,lcr preamble if present
-------------------------------------------------------------------------- */
/* --- reset size, get lcr's, and push exprptr past preamble --- */
exprptr = preamble(subexpr+2,&size,preptr); /* reset size and get lcr's */
/* --- init with global values --- */
for(icol=0; icol<=maxarraysz; icol++) {	/* propagate global values... */
  justify[icol] = gjustify[icol];	/* -1,0,+1 = l,c,r */
  colwidth[icol] = gcolwidth[icol];	/* column width */
  rowheight[icol] = growheight[icol];	/* row height */
  fixcolsize[icol] = gfixcolsize[icol];	/* 1=fixed col width */
  fixrowsize[icol] = gfixrowsize[icol];	/* 1=fixed row height */
  rowcenter[icol] = growcenter[icol]; }	/* true = vcenter row */
/* --- process lcr's, etc in preamble --- */
itoken = 0;				/* debugging flag */
if ( msglevel>=29 && msgfp!=NULL )	/* debugging, display preamble */
 if ( *preptr != '\000' )		/* if we have one */
  fprintf(msgfp,"rastarray> preamble= \"%.256s\"\nrastarray> preamble: ",
  preptr);
irow = icol = 0;			/* init lcr counts */
while (  *preptr != '\000' )		/* check preamble text for lcr */
  {
  char	prepchar = *preptr;		/* current preamble character */
  int	prepcase = (islower(prepchar)?1:(isupper(prepchar)?2:0)); /*1,2,or 0*/
  if ( irow<maxarraysz && icol<maxarraysz )
   switch ( /*tolower*/(prepchar) )
    {  default: break;			/* just flush unrecognized chars */
      case 'l': justify[icol] = (-1);		/*left-justify this column*/
		if (colglobal) gjustify[irow] = justify[irow]; break;
      case 'c': justify[icol] = (0);		/* center this column */
		if (colglobal) gjustify[irow] = justify[irow]; break;
      case 'r': justify[icol] = (+1);		/* right-justify this col */
		if (colglobal) gjustify[irow] = justify[irow]; break;
      case '|': vline[icol] += 1;   break;	/* solid vline left of col */
      case '.': vline[icol] = (-1); break;	/*dashed vline left of col */
      case 'b': prepchar='B'; prepcase=2;	/* alias for B */
      case 'B': break;				/* baseline-justify row */
      case 'v': prepchar='C'; prepcase=2;	/* alias for C */
      case 'C': rowcenter[irow] = 1;		/* vertically center row */
		if (rowglobal) growcenter[irow] = rowcenter[irow]; break;
      case 'g': colglobal=1; prepcase=0; break;	/* set global col values */
      case 'G': rowglobal=1; prepcase=0; break;	/* set global row values */
      case '#': colglobal=rowglobal=1; break; }	/* set global col,row vals */
  if ( msglevel>=29 && msgfp!=NULL )	/* debugging */
    fprintf(msgfp," %c[%d]",prepchar,
    (prepcase==1?icol+1:(prepcase==2?irow+1:0)));
  preptr++;				/* check next char for lcr */
  itoken++;				/* #lcr's processed (debugging only)*/
  /* --- check for number or +number specifying colwidth or rowheight --- */
  if ( prepcase != 0 )			/* only check upper,lowercase */
   {
   int	ispropagate = (*preptr=='+'?1:0); /* leading + propagates width/ht */
   if ( ispropagate ) {			/* set row or col propagation */
     if ( prepcase == 1 ) colpropagate = 1; /* propagating col values */
     else if ( prepcase == 2 ) rowpropagate = 1; } /*propagating row values*/
   if ( !colpropagate && prepcase == 1 )
      {	colwidth[icol] = 0;		/* reset colwidth */
	fixcolsize[icol] = 0; }		/* reset width flag */
   if ( !rowpropagate && prepcase == 2 )
      {	rowheight[irow] = 0;		/* reset row height */
	fixrowsize[irow] = 0; }		/* reset height flag */
   if ( ispropagate ) preptr++;		/* bump past leading + */
   if ( isdigit(*preptr) )		/* digit follows character */
     { char *endptr = NULL;		/* preptr set to 1st char after num*/
       int size = (int)(strtol(preptr,&endptr,10)); /* interpret number */
       char *whchars="?wh";		/* debugging width/height labels */
       preptr = endptr;			/* skip over all digits */
       if ( size==0 || (size>=3&&size<=500) ) { /* sanity check */
	int index;			/* icol,irow...maxarraysz index */
	if ( prepcase == 1 )		/* lowercase signifies colwidth */
	 for(index=icol; index<=maxarraysz; index++) { /*propagate col size*/
	  colwidth[index] = size;	/* set colwidth to fixed size */
	  fixcolsize[index] = (size>0?1:0); /* set fixed width flag */
	  justify[index] = justify[icol]; /* and propagate justification */
	  if ( colglobal ) {		/* set global values */
	    gcolwidth[index] = colwidth[index]; /* set global col width */
	    gfixcolsize[index] = fixcolsize[index]; /*set global width flag*/
	    gjustify[index] = justify[icol]; } /* set global col justify */
	  if ( !ispropagate ) break; }	/* don't propagate */
	else				/* uppercase signifies rowheight */
	 for(index=irow; index<=maxarraysz; index++) { /*propagate row size*/
	  rowheight[index] = size;	/* set rowheight to size */
	  fixrowsize[index] = (size>0?1:0); /* set fixed height flag */
	  rowcenter[index] = rowcenter[irow]; /* and propagate row center */
	  if ( rowglobal ) {		/* set global values */
	    growheight[index] = rowheight[index]; /* set global row height */
	    gfixrowsize[index] = fixrowsize[index]; /*set global height flag*/
	    growcenter[index] = rowcenter[irow]; } /*set global row center*/
	  if ( !ispropagate ) break; }	/* don't propagate */
        } /* --- end-of-if(size>=3&&size<=500) --- */
       if ( msglevel>=29 && msgfp!=NULL ) /* debugging */
	 fprintf(msgfp,":%c=%d/fix#%d",whchars[prepcase],
	 (prepcase==1?colwidth[icol]:rowheight[irow]),
	 (prepcase==1?fixcolsize[icol]:fixrowsize[irow]));
     } /* --- end-of-if(isdigit()) --- */
   } /* --- end-of-if(prepcase!=0) --- */
  if ( prepcase == 1 ) icol++;		/* bump col if lowercase lcr */
    else if ( prepcase == 2 ) irow++;	/* bump row if uppercase BC */
  } /* --- end-of-while(*preptr!='\000') --- */
if ( msglevel>=29 && msgfp!=NULL )	/* debugging, emit final newline */
 if ( itoken > 0 )			/* if we have preamble */
  fprintf(msgfp,"\n");
/* -------------------------------------------------------------------------
tokenize and rasterize components  a & b \\ c & d \\ etc  of subexpr
-------------------------------------------------------------------------- */
/* --- rasterize tokens one at a time, and maintain row,col counts --- */
nrows = 0;				/* start with top row */
ncols[nrows] = 0;			/* no tokens/cols in top row yet */
while ( 1 )				/* scan chars till end */
  {
  /* --- local control flags --- */
  int	iseox = (*exprptr == '\000'),	/* null signals end-of-expression */
	iseor = iseox,			/* \\ or eox signals end-of-row */
	iseoc = iseor;			/* & or eor signals end-of-col */
  /* --- check for escapes --- */
  isescape = isthischar(*exprptr,ESCAPE); /* is current char escape? */
  wasescape= (!isnewrow&&isthischar(*(exprptr-1),ESCAPE)); /*prev char esc?*/
  nescapes = (wasescape?nescapes+1:0);	/* # preceding consecutive escapes */
  ischarescaped = (nescapes%2==0?0:1);	/* is current char escaped? */
  /* -----------------------------------------------------------------------
  check for {...} subexpression starting from where we are now
  ------------------------------------------------------------------------ */
  if ( *exprptr == '{'			/* start of {...} subexpression */
  &&   !ischarescaped )			/* if not escaped \{ */
    {
    subptr = texsubexpr(exprptr,subtok,4095,"{","}",1,1); /*entire subexpr*/
    subtoklen = strlen(subtok);		/* #chars in {...} */
    memcpy(tokptr,exprptr,subtoklen);	/* copy {...} to accumulated token */
    tokptr  += subtoklen;		/* bump tokptr to end of token */
    exprptr += subtoklen;		/* and bump exprptr past {...} */
    istokwhite = 0;			/* signal non-empty token */
    continue;				/* continue with char after {...} */
    } /* --- end-of-if(*exprptr=='{') --- */
  /* -----------------------------------------------------------------------
  check for end-of-row(\\) and/or end-of-col(&)
  ------------------------------------------------------------------------ */
  /* --- check for (escaped) end-of-row delimiter --- */
  if ( isescape && !ischarescaped )	/* current char is escaped */
    if ( isthischar(*(exprptr+1),rowdelim) /* next char is rowdelim */
    ||   *(exprptr+1) == '\000' )	/* or a pathological null */
      {	iseor = 1;			/* so set end-of-row flag */
	wasescape=isescape=nescapes = 0; } /* reset flags for new row */
  /* --- check for end-of-col delimiter --- */
  if (iseor				/* end-of-row signals end-of-col */
  ||  (!ischarescaped&&isthischar(*exprptr,coldelim))) /*or unescaped coldel*/
      iseoc = 1;			/* so set end-of-col flag */
  /* -----------------------------------------------------------------------
  rasterize completed token
  ------------------------------------------------------------------------ */
  if ( iseoc )				/* we have a completed token */
    {
    *tokptr = '\000';			/* first, null-terminate token */
    /* --- check first token in row for [len] and/or \hline or \hdash --- */
    ishonly = 0;			/*init for token not only an \hline*/
    if ( ncols[nrows] == 0 )		/*\hline must be first token in row*/
      {
      tokptr=token; skipwhite(tokptr);	/* skip whitespace after // */
      /* --- first check for optional [len] --- */
      if ( *tokptr == '[' ) {		/* have [len] if leading char is [ */
        /* ---parse [len] and bump tokptr past it, interpret as double--- */
        char lenexpr[128];  int len;	/* chars between [...] as int */
        tokptr = texsubexpr(tokptr,lenexpr,127,"[","]",0,0);
        if ( *lenexpr != '\000' ) {	/* got [len] expression */
	  evalue = evalterm(mimestore,lenexpr); /* evaluate len expression */
          len = iround(unitlength*((double)evalue)); /* len in pixels */
          if ( len>=(-63) && len<=255 ) { /* sanity check */
            vrowspace[nrows] = len;	/* extra vspace before this row */
	    strsqueezep(token,tokptr);	/* flush [len] from token */
            tokptr=token; skipwhite(tokptr); } } /* reset ptr, skip white */
        } /* --- end-of-if(*tokptr=='[') --- */
      /* --- now check for \hline or \hdash --- */
      tokptr = texchar(tokptr,hltoken);	/* extract first char from token */
      hltoklen = strlen(hltoken);	/* length of first char */
      if ( hltoklen >= minhltoklen ) {	/*token must be at least \hl or \hd*/
	if ( memcmp(hlchar,hltoken,hltoklen) == 0 ) /* we have an \hline */
	   hline[nrows] += 1;		/* bump \hline count for row */
	else if ( memcmp(hdchar,hltoken,hltoklen) == 0 ) /*we have an \hdash*/
	   hline[nrows] = (-1); }	/* set \hdash flag for row */
      if ( hline[nrows] != 0 )		/* \hline or \hdash prefixes token */
	{ skipwhite(tokptr);		/* flush whitespace after \hline */
	  if ( *tokptr == '\000'	/* end-of-expression after \hline */
	  ||   isthischar(*tokptr,coldelim) ) /* or unescaped coldelim */
	    { istokwhite = 1;		/* so token contains \hline only */
	      if ( iseox ) ishonly = 1; } /* ignore entire row at eox */
	  else				/* token contains more than \hline */
	    {strsqueezep(token,tokptr);} } /* so flush \hline */
      } /* --- end-of-if(ncols[nrows]==0) --- */
    /* --- rasterize completed token --- */
    toksp[ntokens] = (istokwhite? NULL : /* don't rasterize empty token */
      rasterize(token,size));		/* rasterize non-empty token */
    if ( toksp[ntokens] != NULL )	/* have a rasterized token */
      nnonwhite++;			/* bump rasterized token count */
    /* --- maintain colwidth[], rowheight[] max, and rowbaseln[] --- */
    if ( toksp[ntokens] != NULL )	/* we have a rasterized token */
      {
      /* --- update max token "height" in current row, and baseline --- */
      int twidth = ((toksp[ntokens])->image)->width,  /* width of token */
	theight = ((toksp[ntokens])->image)->height, /* height of token */
	tbaseln =  (toksp[ntokens])->baseline, /* baseline of token */
	rheight = rowheight[nrows],	/* current max height for row */
	rbaseln = rowbaseln[nrows];	/* current baseline for max height */
      if ( 0 || fixrowsize[nrows]==0 )	/* rowheight not fixed */
       rowheight[nrows] = /*max2( rheight,*/( /* current (max) rowheight */
	max2(rbaseln+1,tbaseln+1)	/* max height above baseline */
	+ max2(rheight-rbaseln-1,theight-tbaseln-1) ); /* plus max below */
      rowbaseln[nrows] = max2(rbaseln,tbaseln); /*max space above baseline*/
      /* --- update max token width in current column --- */
      icol = ncols[nrows];		/* current column index */
      if ( 0 || fixcolsize[icol]==0 )	/* colwidth not fixed */
       colwidth[icol] = max2(colwidth[icol],twidth); /*widest token in col*/
      } /* --- end-of-if(toksp[]!=NULL) --- */
    /* --- bump counters --- */
    if ( !ishonly )			/* don't count only an \hline */
      if ( ncols[nrows] < maxarraysz )	/* don't overflow arrays */
	{ ntokens++;			/* bump total token count */
	  ncols[nrows] += 1; }		/* and bump #cols in current row */
    /* --- get ready for next token --- */
    tokptr = token;			/* reset ptr for next token */
    istokwhite = 1;			/* next token starts all white */
    } /* --- end-of-if(iseoc) --- */
  /* -----------------------------------------------------------------------
  bump row as necessary
  ------------------------------------------------------------------------ */
  if ( iseor )				/* we have a completed row */
    {
    maxcols = max2(maxcols,ncols[nrows]); /* max# cols in array */
    if ( ncols[nrows]>0 || hline[nrows]==0 ) /*ignore row with only \hline*/
      if ( nrows < maxarraysz )		/* don't overflow arrays */
        nrows++;			/* bump row count */
    ncols[nrows] = 0;			/* no cols in this row yet */
    if ( !iseox )			/* don't have a null yet */
      {	exprptr++;			/* bump past extra \ in \\ delim */
	iseox = (*exprptr == '\000'); }	/* recheck for pathological \null */
    isnewrow = 1;			/* signal start of new row */
    } /* --- end-of-if(iseor) --- */
  else
    isnewrow = 0;			/* no longer first col of new row */
  /* -----------------------------------------------------------------------
  quit when done, or accumulate char in token and proceed to next char
  ------------------------------------------------------------------------ */
  /* --- quit when done --- */
  if ( iseox ) break;			/* null terminator signalled done */
  /* --- accumulate chars in token --- */
  if ( !iseoc )				/* don't accumulate delimiters */
    { *tokptr++ = *exprptr;		/* accumulate non-delim char */
      if ( !isthischar(*exprptr,WHITESPACE) ) /* this token isn't empty */
	istokwhite = 0; }		/* so reset flag to rasterize it */
  /* --- ready for next char --- */
  exprptr++;				/* bump ptr */
  } /* --- end-of-while(*exprptr!='\000') --- */
/* --- make sure we got something to do --- */
if ( nnonwhite < 1 )			/* completely empty array */
  goto end_of_job;			/* NULL back to caller */
/* -------------------------------------------------------------------------
determine dimensions of array raster and allocate it
-------------------------------------------------------------------------- */
/* --- adjust colspace --- */
colspace = 2 + 2*size;			/* temp kludge */
/* --- reset propagated sizes at boundaries of array --- */
colwidth[maxcols] = rowheight[nrows] = 0; /* reset explicit 0's at edges */
/* --- determine width of array raster --- */
width = colspace*(maxcols-1);		/* empty space between cols */
if ( msglevel>=29 && msgfp!=NULL )	/* debugging */
  fprintf(msgfp,"rastarray> %d cols,  widths: ",maxcols);
for ( icol=0; icol<=maxcols; icol++ )	/* and for each col */
  { width += colwidth[icol];		/*width of this col (0 for maxcols)*/
    width += vlinespace(icol);		/*plus space for vline, if present*/
    if ( msglevel>=29 && msgfp!=NULL )	/* debugging */
     fprintf(msgfp," %d=%2d+%d",icol+1,colwidth[icol],(vlinespace(icol))); }
/* --- determine height of array raster --- */
height = rowspace*(nrows-1);		/* empty space between rows */
if ( msglevel>=29 && msgfp!=NULL )	/* debugging */
  fprintf(msgfp,"\nrastarray> %d rows, heights: ",nrows);
for ( irow=0; irow<=nrows; irow++ )	/* and for each row */
  { height += rowheight[irow];		/*height of this row (0 for nrows)*/
    height += vrowspace[irow];		/*plus extra //[len], if present*/
    height += hlinespace(irow);		/*plus space for hline, if present*/
    if ( msglevel>=29 && msgfp!=NULL )	/* debugging */
     fprintf(msgfp," %d=%2d+%d",irow+1,rowheight[irow],(hlinespace(irow))); }
/* --- allocate subraster and raster for array --- */
if ( msglevel>=29 && msgfp!=NULL )	/* debugging */
  fprintf(msgfp,"\nrastarray> tot width=%d(colspc=%d) height=%d(rowspc=%d)\n",
  width,colspace, height,rowspace);
if ( (arraysp=new_subraster(width,height,pixsz)) /* allocate new subraster */
==   NULL )  goto end_of_job;		/* quit if failed */
/* --- initialize subraster parameters --- */
arraysp->type = IMAGERASTER;		/* image */
arraysp->symdef = NULL;			/* not applicable for image */
arraysp->baseline=min2(height/2+5,height-1); /*is a little above center good?*/
arraysp->size = size;			/* size (probably unneeded) */
arrayrp = arraysp->image;		/* raster embedded in subraster */
/* -------------------------------------------------------------------------
embed tokens/cells in array
-------------------------------------------------------------------------- */
itoken = 0;				/* start with first token */
toprow = 0;				/* start at top row of array */
for ( irow=0; irow<=nrows; irow++ )	/*tokens were accumulated row-wise*/
  {
  /* --- initialization for row --- */
  int	baseline = rowbaseln[irow];	/* baseline for this row */
  if ( hline[irow] != 0 )		/* need hline above this row */
    { int hrow = (irow<1? 0 : toprow - rowspace/2); /* row for hline */
      if ( irow >= nrows ) hrow = height-1; /* row for bottom hline */
      rule_raster(arrayrp,hrow,0,width,1,(hline[irow]<0?1:0)); } /* hline */
  if ( irow >= nrows ) break;		/*just needed \hline for irow=nrows*/
  toprow += vrowspace[irow];		/* extra //[len] space above irow */
  if ( toprow < 0 ) toprow = 0;		/* check for large negative [-len] */
  toprow += hlinespace(irow);		/* space for hline above irow */
  leftcol = 0;				/* start at leftmost column */
  for ( icol=0; icol<ncols[irow]; icol++ ) /* go through cells in this row */
    {
    subraster *tsp = toksp[itoken];	/* token that belongs in this cell */
    /* --- first adjust leftcol for vline to left of icol, if present ---- */
    leftcol += vlinespace(icol);	/* space for vline to left of col */
    /* --- now rasterize cell ---- */
    if ( tsp != NULL )			/* have a rasterized cell token */
      {
      /* --- local parameters --- */
      int cwidth = colwidth[icol],	/* total column width */
	  twidth = (tsp->image)->width,	/* token width */
	  theight= (tsp->image)->height, /* token height */
	  tokencol = 0,			/*H offset (init for left justify)*/
	  tokenrow = baseline - tsp->baseline;/*V offset (init for baseline)*/
      /* --- adjust leftcol for vline to left of icol, if present ---- */
      /*leftcol += vlinespace(icol);*/	/* space for vline to left of col */
      /* --- reset justification (if not left-justified) --- */
      if ( justify[icol] == 0 )		/* but user wants it centered */
	  tokencol = (cwidth-twidth+1)/2; /* so split margin left/right */
      else if ( justify[icol] == 1 )	/* or user wants right-justify */
	  tokencol = cwidth-twidth;	/* so put entire margin at left */
      /* --- reset vertical centering (if not baseline-aligned) --- */
      if ( rowcenter[irow] )		/* center cells in row vertically */
	  tokenrow = (rowheight[irow]-theight)/2; /* center row */
      /* --- embed token raster at appropriate place in array raster --- */
      rastput(arrayrp,tsp->image,	/* overlay cell token in array */
	  toprow+ tokenrow,		/*with aligned baseline or centered*/
	  leftcol+tokencol, 1);		/* and justified as requested */
      } /* --- end-of-if(tsp!=NULL) --- */
    itoken++;				/* bump index for next cell */
    leftcol += colwidth[icol] + colspace /*move leftcol right for next col*/
      /* + vlinespace(icol) */ ; /*don't add space for vline to left of col*/
    } /* --- end-of-for(icol) --- */
  toprow += rowheight[irow] + rowspace;	/* move toprow down for next row */
  } /* --- end-of-for(irow) --- */
/* -------------------------------------------------------------------------
draw vlines as necessary
-------------------------------------------------------------------------- */
leftcol = 0;				/* start at leftmost column */
for ( icol=0; icol<=maxcols; icol++ )	/* check each col for a vline */
  {
  if ( vline[icol] != 0 )		/* need vline to left of this col */
    { int vcol = (icol<1? 0 : leftcol - colspace/2); /* column for vline */
      if ( icol >= maxcols ) vcol = width-1; /*column for right edge vline*/
      rule_raster(arrayrp,0,vcol,1,height,(vline[icol]<0?2:0)); } /* vline */
  leftcol += vlinespace(icol);		/* space for vline to left of col */
  if ( icol < maxcols )			/* don't address past end of array */
    leftcol += colwidth[icol] + colspace; /*move leftcol right for next col*/
  } /* --- end-of-for(icol) --- */
/* -------------------------------------------------------------------------
free workspace and return final result to caller
-------------------------------------------------------------------------- */
end_of_job:
  /* --- free workspace --- */
  if ( ntokens > 0 )			/* if we have workspace to free */
    while ( --ntokens >= 0 )		/* free each token subraster */
      if ( toksp[ntokens] != NULL )	/* if we rasterized this cell */
	delete_subraster(toksp[ntokens]); /* then free it */
  /* --- return final result to caller --- */
  return ( arraysp );
} /* --- end-of-function rastarray() --- */


/* ==========================================================================
 * Function:	rastpicture ( expression, size, basesp, arg1, arg2, arg3 )
 * Purpose:	\picture handler, returns subraster corresponding to picture
 *		expression (immediately following \picture) at font size
 * --------------------------------------------------------------------------
 * Arguments:	expression (I/O) char **  to first char of null-terminated
 *				string immediately following \picture to be
 *				rasterized, and returning ptr immediately
 *				following last character processed.
 *		size (I)	int containing 0-7 default font size
 *		basesp (I)	subraster *  to character (or subexpression)
 *				immediately preceding \picture
 *				(unused, but passed for consistency)
 *		arg1 (I)	int unused
 *		arg2 (I)	int unused
 *		arg3 (I)	int unused
 * --------------------------------------------------------------------------
 * Returns:	( subraster * )	ptr to subraster corresponding to picture
 *				expression, or NULL for any parsing error
 * --------------------------------------------------------------------------
 * Notes:     o	Summary of syntax...
 *		  \picture(width,height){(x,y){pic_elem}~(x,y){pic_elem}~etc}
 *	      o	
 * ======================================================================= */
/* --- entry point --- */
subraster *rastpicture ( char **expression, int size, subraster *basesp,
			int arg1, int arg2, int arg3 )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
char	*texsubexpr(), picexpr[2049], *picptr=picexpr, /* picture {expre} */
	putexpr[256], *putptr,*multptr,	/*[multi]put (x,y[;xinc,yinc;num])*/
	pream[96], *preptr,		/* optional put preamble */
	picelem[1025];			/* picture element following put */
subraster   *rasterize(), *picelemsp=NULL, /* rasterize picture elements */
	*new_subraster(), *picturesp=NULL, /* subraster for entire picture */
	*oldworkingbox = workingbox;	/* save working box on entry */
raster	*picturerp=NULL;		/* raster for entire picture */
int	delete_subraster();		/* free picelemsp[] workspace */
int	pixsz = 1;			/* pixels are one bit each */
double	x=0.0,y=0.0,			/* x,y-coords for put,multiput*/
	xinc=0.0,yinc=0.0;		/* x,y-incrementss for multiput*/
int	width=0,  height=0,		/* #pixels width,height of picture */
	ewidth=0, eheight=0,		/* pic element width,height */
	maxwidth=1600, maxheight=1600,	/* max width,height in pixels */
	ix=0,xpos=0, iy=0,ypos=0,	/* mimeTeX x,y pixel coords */
	num=1, inum;			/* number reps, index of element */
int	evalterm();			/* evaluate [arg] and {arg}'s */
int	iscenter=0;			/* center or lowerleft put position*/
int	*oldworkingparam = workingparam, /* save working param on entry */
	origin = 0;			/* x,yinc ++=00 +-=01 -+=10 --=11 */
int	rastput();			/* embed elements in picture */
int	type_raster();			/* display debugging output */
/* -------------------------------------------------------------------------
First obtain (width,height) arguments immediately following \picture command
-------------------------------------------------------------------------- */
/* --- parse for (width,height) arguments, and bump expression past it --- */
*expression = texsubexpr(*expression,putexpr,254,"(",")",0,0);
if ( *putexpr == '\000' ) goto end_of_job; /* couldn't get (width,height) */
/* --- now interpret width,height returned in putexpr --- */
if ( (putptr=strchr(putexpr,',')) != NULL ) /* look for ',' in width,height*/
  *putptr = '\000';			/* found it, so replace ',' by '\0'*/
width=height = eround(putexpr);		/*width pixels*/
if ( putptr != NULL )			/* 2nd arg, if present, is height */
  height = eround(putptr+1);		/*in pixels*/
/* -------------------------------------------------------------------------
Then obtain entire picture {...} subexpression following (width,height)
-------------------------------------------------------------------------- */
/* --- parse for picture subexpression, and bump expression past it --- */
*expression = texsubexpr(*expression,picexpr,2047,"{","}",0,0);
if ( *picexpr == '\000' ) goto end_of_job; /* couldn't get {pic_elements} */
/* -------------------------------------------------------------------------
allocate subraster and raster for complete picture
-------------------------------------------------------------------------- */
/* --- sanity check on width,height args --- */
if ( width < 2 ||  width > maxwidth
||  height < 2 || height > maxheight ) goto end_of_job;
/* --- allocate and initialize subraster for constructed picture --- */
if ( (picturesp=new_subraster(width,height,pixsz)) /*allocate new subraster*/
==   NULL )  goto end_of_job;		/* quit if failed */
workingbox = picturesp;			/* set workingbox to our picture */
/* --- initialize picture subraster parameters --- */
picturesp->type = IMAGERASTER;		/* image */
picturesp->symdef = NULL;		/* not applicable for image */
picturesp->baseline = height/2 + 2;	/* is a little above center good? */
picturesp->size = size;			/* size (probably unneeded) */
picturerp = picturesp->image;		/* raster embedded in subraster */
if ( msgfp!=NULL && msglevel>=29 )	/* debugging */
  fprintf(msgfp,"picture> width,height=%d,%d\n",width,height);
/* -------------------------------------------------------------------------
parse out each picture element, rasterize it, and place it in picture
-------------------------------------------------------------------------- */
while ( *picptr != '\000' )		/* until we run out of pic_elems */
  {
  /* -----------------------------------------------------------------------
  first obtain leading \[multi]put(x,y[;xinc,yinc;num]) args for pic_elem
  ------------------------------------------------------------------------ */
  /* --- init default values in case not explicitly supplied in args --- */
  x=y=0.0;  xinc=yinc=0.0;  num=1;	/* init default values */
  iscenter = origin = 0;		/* center, origin */
  /* --- get (pream$x,y;xinc,yinc;num ) args and bump picptr past it --- */
  while ( *picptr != '\000' )		/* skip invalid chars preceding ( */
    if ( *picptr == '(' ) break;	/* found opening ( */
    else picptr++;			/* else skip invalid char */
  picptr = texsubexpr(picptr,putexpr,254,"(",")",0,0);
  if ( *putexpr == '\000' ) goto end_of_job; /* couldn't get (x,y) */
  /* --- first look for $-terminated or for any non-digit preamble --- */
  *pream = '\000';			/* init preamble as empty string */
  if ( (putptr=strchr(putexpr,'$')) != NULL ) /*check for $ pream terminator*/
    { *putptr++ = '\000';		/* replace $ by '\0', bump past $ */
      strninit(pream,putexpr,92); }	/* copy leading preamble from put */
  else					/* look for any non-digit preamble */
    { int npream = 0;			/* #chars in preamble */
      for ( preptr=pream,putptr=putexpr; ; npream++,putptr++ )
	if ( *putptr == '\000'		/* end-of-putdata signalled */
	||   !isalpha((int)(*putptr))	/* or found non-alpha char */
	||   npream > 92 ) break;	/* or preamble too long */
	else *preptr++ = *putptr;	/* copy alpha char to preamble */
      *preptr = '\000'; }		/* null-terminate preamble */
  /* --- interpret preamble --- */
  for ( preptr=pream; ; preptr++ )	/* examine each preamble char */
    if ( *preptr == '\000' ) break;	/* end-of-preamble signalled */
    else switch ( tolower(*preptr) )	/* check lowercase preamble char */
      {
      default: break;			/* unrecognized flag */
      case 'c': iscenter=1; break;	/* center pic_elem at x,y coords */
      } /* --- end-of-switch --- */
  /* --- interpret x,y;xinc,yinc;num following preamble --- */      
  if ( *putptr != '\000' )		/*check for put data after preamble*/
   {
   /* --- first squeeze preamble out of put expression --- */
   if ( *pream != '\000' )		/* have preamble */
     {strsqueezep(putexpr,putptr);}	/* squeeze it out */
   /* --- interpret x,y --- */
   if ( (multptr=strchr(putexpr,';')) != NULL ) /*semicolon signals multiput*/
     *multptr = '\000';			/* replace semicolon by '\0' */
   if ( (putptr=strchr(putexpr,',')) != NULL ) /* comma separates x,y */
     *putptr = '\000';			/* replace comma by '\0'  */
   if ( *putexpr != '\000' )		/* leading , may be placeholder */
     x = (double)(eround(putexpr));	/* x coord in pixels*/
   if ( putptr != NULL )		/* 2nd arg, if present, is y coord */
     y = (double)(eround(putptr+1));	/* in pixels */
   /* --- interpret xinc,yinc,num if we have a multiput --- */
   if ( multptr != NULL )		/* found ';' signalling multiput */
     {
     if ( (preptr=strchr(multptr+1,';')) != NULL ) /* ';' preceding num arg*/
       *preptr = '\000';		/* replace ';' by '\0' */
     if ( (putptr=strchr(multptr+1,',')) != NULL ) /* ',' between xinc,yinc*/
       *putptr = '\000';		/* replace ',' by '\0' */
     if ( *(multptr+1) != '\000' )	/* leading , may be placeholder */
       xinc = (double)(eround(multptr+1)); /* xinc in pixels */
     if ( putptr != NULL )		/* 2nd arg, if present, is yinc */
       yinc = (double)(eround(putptr+1)); /* in user pixels */
     num = (preptr==NULL? 999 : atoi(preptr+1)); /*explicit num val or 999*/
     } /* --- end-of-if(multptr!=NULL) --- */
   } /* --- end-of-if(*preptr!='\000') --- */
  if ( msgfp!=NULL && msglevel>=29 )	/* debugging */
    fprintf(msgfp,
    "picture> pream;x,y;xinc,yinc;num=\"%s\";%.2f,%.2f;%.2f,%.2f;%d\n",
    pream,x,y,xinc,yinc,num);
  /* -----------------------------------------------------------------------
  now obtain {...} picture element following [multi]put, and rasterize it
  ------------------------------------------------------------------------ */
  /* --- parse for {...} picture element and bump picptr past it --- */
  picptr = texsubexpr(picptr,picelem,1023,"{","}",0,0);
  if ( *picelem == '\000' ) goto end_of_job; /* couldn't get {pic_elem} */
  if ( msgfp!=NULL && msglevel>=29 )	/* debugging */
    fprintf(msgfp, "picture> picelem=\"%.50s\"\n",picelem);
  /* --- rasterize picture element --- */
  origin = 0;				/* init origin as working param */
  workingparam = &origin;		/* and point working param to it */
  picelemsp = rasterize(picelem,size);	/* rasterize picture element */
  if ( picelemsp == NULL ) continue;	/* failed to rasterize, skip elem */
  ewidth  = (picelemsp->image)->width;	/* width of element, in pixels */
  eheight = (picelemsp->image)->height;	/* height of element, in pixels */
  if ( origin == 55 ) iscenter = 1;	/* origin set to (.5,.5) for center*/
  if ( msgfp!=NULL && msglevel>=29 )	/* debugging */
    { fprintf(msgfp, "picture> ewidth,eheight,origin,num=%d,%d,%d,%d\n",
      ewidth,eheight,origin,num);
      if ( msglevel >= 999 ) type_raster(picelemsp->image,msgfp); }
  /* -----------------------------------------------------------------------
  embed element in picture (once, or multiple times if requested)
  ------------------------------------------------------------------------ */
  for ( inum=0; inum<num; inum++ )	/* once, or for num repetitions */
    {
    /* --- set x,y-coords for this iteration --- */
    ix = iround(x);  iy = iround(y);	/* round x,y to nearest integer */
    if ( iscenter )			/* place center of element at x,y */
      {	xpos = ix - ewidth/2;		/* x picture coord to center elem */
	ypos = height - iy - eheight/2; } /* y pixel coord to center elem */
    else				/* default places lower-left at x,y*/
      {	xpos = ix;			/* set x pixel coord for left */
	if ( origin==10 || origin==11 )	/* x,yinc's are -+ or -- */
	  xpos = ix - ewidth;		/* so set for right instead */
	ypos = height - iy - eheight;	/* set y pixel coord for lower */
	if ( origin==1 || origin==11 )	/* x,yinc's are +- or -- */
	  ypos = height - iy; }		/* so set for upper instead */
    if ( msgfp!=NULL && msglevel>=29 )	/* debugging */
      fprintf(msgfp,
      "picture> inum,x,y,ix,iy,xpos,ypos=%d,%.2f,%.2f,%d,%d,%d,%d\n",
      inum,x,y,ix,iy,xpos,ypos);
    /* --- embed token raster at xpos,ypos, and quit if out-of-bounds --- */
    if ( !rastput(picturerp,picelemsp->image,ypos,xpos,0) ) break;
    /* --- apply increment --- */
    if ( xinc==0. && yinc==0. ) break;	/* quit if both increments zero */
    x += xinc;  y += yinc;		/* increment coords for next iter */
    } /* --- end-of-for(inum) --- */
  /* --- free picture element subraster after embedding it in picture --- */
  delete_subraster(picelemsp);		/* done with subraster, so free it */
  } /* --- end-of-while(*picptr!=0) --- */
/* -------------------------------------------------------------------------
return picture constructed from pic_elements to caller
-------------------------------------------------------------------------- */
end_of_job:
  workingbox = oldworkingbox;		/* restore original working box */
  workingparam = oldworkingparam;	/* restore original working param */
  return ( picturesp );			/* return our picture to caller */
} /* --- end-of-function rastpicture() --- */


/* ==========================================================================
 * Function:	rastline ( expression, size, basesp, arg1, arg2, arg3 )
 * Purpose:	\line handler, returns subraster corresponding to line
 *		parameters (xinc,yinc){xlen}
 * --------------------------------------------------------------------------
 * Arguments:	expression (I/O) char **  to first char of null-terminated
 *				string immediately following \line to be
 *				rasterized, and returning ptr immediately
 *				following last character processed.
 *		size (I)	int containing 0-7 default font size
 *		basesp (I)	subraster *  to character (or subexpression)
 *				immediately preceding \line
 *				(unused, but passed for consistency)
 *		arg1 (I)	int unused
 *		arg2 (I)	int unused
 *		arg3 (I)	int unused
 * --------------------------------------------------------------------------
 * Returns:	( subraster * )	ptr to subraster corresponding to line
 *				requested, or NULL for any parsing error
 * --------------------------------------------------------------------------
 * Notes:     o	Summary of syntax...
 *		  \line(xinc,yinc){xlen}
 *	      o	if {xlen} not given, then it's assumed xlen = |xinc|
 * ======================================================================= */
/* --- entry point --- */
subraster *rastline ( char **expression, int size, subraster *basesp,
			int arg1, int arg2, int arg3 )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
char	*texsubexpr(),linexpr[257], *xptr=linexpr; /*line(xinc,yinc){xlen}*/
subraster *new_subraster(), *linesp=NULL; /* subraster for line */
/*char	*origexpression = *expression;*/ /*original expression after \line*/
int	pixsz = 1;			/* pixels are one bit each */
int	thickness = 1;			/* line thickness */
double	xinc=0.0, yinc=0.0,		/* x,y-increments for line, */
	xlen=0.0, ylen=0.0;		/* x,y lengths for line */
int	width=0,  height=0,		/* #pixels width,height of line */
	maxwidth=1600, maxheight=1600,	/* max width,height in pixels */
	rwidth=0, rheight=0;		/*alloc width,height plus thickness*/
int	evalterm();			/* evaluate [arg] and {arg}'s */
int	istop=0,  isright=0,		/* origin at bot-left if x,yinc>=0 */
	origin = 0;			/* x,yinc: ++=00 +-=01 -+=10 --=11 */
int	line_raster();			/* draw line in linesp->image */
/* -------------------------------------------------------------------------
obtain (xinc,yinc) arguments immediately following \line command
-------------------------------------------------------------------------- */
/* --- parse for (xinc,yinc) arguments, and bump expression past it --- */
*expression = texsubexpr(*expression,linexpr,253,"(",")",0,0);
if ( *linexpr == '\000' ) goto end_of_job; /* couldn't get (xinc,yinc) */
/* --- now interpret xinc,yinc;thickness returned in linexpr --- */
if ( (xptr=strchr(linexpr,';')) != NULL ) /* look for ';' after xinc,yinc */
  { *xptr = '\000';			/* terminate linexpr at ; */
    thickness = evalterm(mimestore,xptr+1); } /* get int thickness */
if ( (xptr=strchr(linexpr,',')) != NULL ) /* look for ',' in xinc,yinc */
  *xptr = '\000';			/* found it, so replace ',' by '\0'*/
if ( *linexpr != '\000' )		/* check against missing 1st arg */
  xinc = xlen = (double)evalterm(mimestore,linexpr); /* xinc in user units */
if ( xptr != NULL )			/* 2nd arg, if present, is yinc */
  yinc = ylen = (double)evalterm(mimestore,xptr+1); /* in user units */
/* -------------------------------------------------------------------------
obtain optional {xlen} following (xinc,yinc), and calculate ylen
-------------------------------------------------------------------------- */
/* --- check if {xlen} given --- */
if ( *(*expression) == '{' )		/*have {xlen} if leading char is { */
  {
  /* --- parse {xlen} and bump expression past it, interpret as double --- */
  *expression = texsubexpr(*expression,linexpr,253,"{","}",0,0);
  if ( *linexpr == '\000' ) goto end_of_job; /* couldn't get {xlen} */
  xlen = (double)evalterm(mimestore,linexpr); /* xlen in user units */
  /* --- set other values accordingly --- */
  if ( xlen  < 0.0 ) xinc = -xinc;	/* if xlen negative, flip xinc sign*/
  if ( xinc != 0.0 ) ylen = xlen*yinc/xinc; /* set ylen from xlen and slope*/
  else xlen  = 0.0;			/* can't have xlen if xinc=0 */
  } /* --- end-of-if(*(*expression)=='{') --- */
/* -------------------------------------------------------------------------
calculate width,height, etc, based on xlen,ylen, etc
-------------------------------------------------------------------------- */
/* --- force lengths positive --- */
xlen = absval(xlen);			/* force xlen positive */
ylen = absval(ylen);			/* force ylen positive */
/* --- calculate corresponding lengths in pixels --- */
width   = max2(1,iround(unitlength*xlen)); /*scale by unitlength and round,*/
height  = max2(1,iround(unitlength*ylen)); /* and must be at least 1 pixel */
rwidth  = width  + (ylen<0.001?0:max2(0,thickness-1));
rheight = height + (xlen<0.001?0:max2(0,thickness-1));
/* --- set origin corner, x,yinc's: ++=0=(0,0) +-=1=(0,1) -+=10=(1,0) --- */
if ( xinc < 0.0 ) isright = 1;		/*negative xinc, so corner is (1,?)*/
if ( yinc < 0.0 ) istop = 1;		/*negative yinc, so corner is (?,1)*/
origin = isright*10 + istop;		/* interpret 0=(0,0), 11=(1,1), etc*/
if ( msgfp!=NULL && msglevel>=29 )	/* debugging */
  fprintf(msgfp,"rastline> width,height,origin;x,yinc=%d,%d,%d;%g,%g\n",
  width,height,origin,xinc,yinc);
/* -------------------------------------------------------------------------
allocate subraster and raster for line
-------------------------------------------------------------------------- */
/* --- sanity check on width,height,thickness args --- */
if ( width < 1 ||  width > maxwidth
||  height < 1 || height > maxheight
||  thickness<1||thickness>25 ) goto end_of_job;
/* --- allocate and initialize subraster for constructed line --- */
if ( (linesp=new_subraster(rwidth,rheight,pixsz)) /* alloc new subraster */
==   NULL )  goto end_of_job;		/* quit if failed */
/* --- initialize line subraster parameters --- */
linesp->type = IMAGERASTER;		/* image */
linesp->symdef = NULL;			/* not applicable for image */
linesp->baseline = height/2 + 2		/* is a little above center good? */
	+ (rheight-height)/2;		/* account for line thickness too */
linesp->size = size;			/* size (probably unneeded) */
/* -------------------------------------------------------------------------
draw the line
-------------------------------------------------------------------------- */
line_raster ( linesp->image,		/* embedded raster image */
	(istop?   0 : height-1),	/* row0, from bottom or top */
	(isright?  width-1 : 0),	/* col0, from left or right */
	(istop?   height-1 : 0),	/* row1, to top or bottom */
	(isright? 0 :  width-1),	/* col1, to right or left */
	thickness );			/* line thickness (usually 1 pixel)*/
/* -------------------------------------------------------------------------
return constructed line to caller
-------------------------------------------------------------------------- */
end_of_job:
  if ( workingparam != NULL )		/* caller wants origin */
    *workingparam = origin;		/* return origin corner to caller */
  return ( linesp );			/* return line to caller */
} /* --- end-of-function rastline() --- */


/* ==========================================================================
 * Function:	rastrule ( expression, size, basesp, arg1, arg2, arg3 )
 * Purpose:	\rule handler, returns subraster corresponding to rule
 *		parameters [lift]{width}{height}
 * --------------------------------------------------------------------------
 * Arguments:	expression (I/O) char **  to first char of null-terminated
 *				string immediately following \rule to be
 *				rasterized, and returning ptr immediately
 *				following last character processed.
 *		size (I)	int containing 0-7 default font size
 *		basesp (I)	subraster *  to character (or subexpression)
 *				immediately preceding \rule
 *				(unused, but passed for consistency)
 *		arg1 (I)	int unused
 *		arg2 (I)	int unused
 *		arg3 (I)	int unused
 * --------------------------------------------------------------------------
 * Returns:	( subraster * )	ptr to subraster corresponding to rule
 *				requested, or NULL for any parsing error
 * --------------------------------------------------------------------------
 * Notes:     o	Summary of syntax...
 *		  \rule[lift]{width}{height}
 *	      o	if [lift] not given, then bottom of rule on baseline
 *	      o	if width=0 then you get an invisible strut 1 (one) pixel wide
 * ======================================================================= */
/* --- entry point --- */
subraster *rastrule ( char **expression, int size, subraster *basesp,
			int arg1, int arg2, int arg3 )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
char	*texsubexpr(), rulexpr[257];	/* rule[lift]{wdth}{hgt} */
subraster *new_subraster(), *rulesp=NULL; /* subraster for rule */
int	pixsz = 1;			/* pixels are one bit each */
int	lift=0, width=0, height=0;	/* default rule parameters */
double	dval;				/* convert ascii params to doubles */
int	rwidth=0, rheight=0,		/* alloc width, height plus lift */
	maxwidth=1600, maxheight=1600;	/* max width,height in pixels */
int	rule_raster();			/* draw rule in rulesp->image */
int	evalterm();			/* evaluate args */
/* -------------------------------------------------------------------------
Obtain lift,width,height
-------------------------------------------------------------------------- */
/* --- check for optional lift arg  --- */
if ( *(*expression) == '[' )		/*check for []-enclosed optional arg*/
  { *expression = texsubexpr(*expression,rulexpr,255,"[","]",0,0);
    dval = evalterm(mimestore,rulexpr);	/* convert [lift] to int */
    if ( dval <= 99 && dval >= (-99) )	/* sanity check */
      lift = iround(unitlength*dval); }	/* scale by unitlength and round */
/* --- parse for width --- */
*expression = texsubexpr(*expression,rulexpr,255,"{","}",0,0);
if ( *rulexpr == '\000' ) goto end_of_job; /* quit if args missing */
dval = evalterm(mimestore,rulexpr);	/* convert {width} to int */
if ( dval <= 500 && dval >= 0 )		/* sanity check */
  width = max2(0,iround(unitlength*dval)); /* scale by unitlength and round*/
/* --- parse for height --- */
*expression = texsubexpr(*expression,rulexpr,255,"{","}",0,0);
if ( *rulexpr == '\000' ) goto end_of_job; /* quit if args missing */
dval = evalterm(mimestore,rulexpr);	/* convert {height} to int */
if ( dval <= 500 && dval > 0 )		/* sanity check */
  height= max2(1,iround(unitlength*dval)); /* scale by unitlength and round*/
/* --- raster width,height in pixels --- */
rwidth  = max2(1,width);		/* raster must be at least 1 pixel*/
rheight = height + (lift>=0?lift:	/* raster height plus lift */
  (-lift<height?0:-lift-height+1));	/* may need empty space above rule */
/* -------------------------------------------------------------------------
allocate subraster and raster for rule
-------------------------------------------------------------------------- */
/* --- sanity check on width,height,thickness args --- */
if ( rwidth < 1 ||  rwidth > maxwidth
||  rheight < 1 || rheight > maxheight ) goto end_of_job;
/* --- allocate and initialize subraster for constructed rule --- */
if ( (rulesp=new_subraster(rwidth,rheight,pixsz)) /* alloc new subraster */
==   NULL )  goto end_of_job;		/* quit if failed */
/* --- initialize line subraster parameters --- */
rulesp->type = IMAGERASTER;		/* image */
rulesp->symdef = NULL;			/* not applicable for image */
rulesp->baseline = rheight-1 + (lift>=0?0:lift); /*adjust baseline for lift*/
rulesp->size = size;			/* size (probably unneeded) */
/* -------------------------------------------------------------------------
draw the rule
-------------------------------------------------------------------------- */
rule_raster ( rulesp->image,		/* embedded raster image */
	(-lift<height?0:rheight-height), /* topmost row for top-left corner*/
	0,				/* leftmost col for top-left corner*/
	width,				/* rule width */
	height,				/* rule height */
	( width>0? 0:4 ) );		/* rule type */
/* -------------------------------------------------------------------------
return constructed rule to caller
-------------------------------------------------------------------------- */
end_of_job:
  return ( rulesp );			/* return rule to caller */
} /* --- end-of-function rastrule() --- */


/* ==========================================================================
 * Function:	rastcircle ( expression, size, basesp, arg1, arg2, arg3 )
 * Purpose:	\circle handler, returns subraster corresponding to ellipse
 *		parameters (xdiam[,ydiam])
 * --------------------------------------------------------------------------
 * Arguments:	expression (I/O) char **  to first char of null-terminated
 *				string immediately following \circle to be
 *				rasterized, and returning ptr immediately
 *				following last character processed.
 *		size (I)	int containing 0-7 default font size
 *		basesp (I)	subraster *  to character (or subexpression)
 *				immediately preceding \circle
 *				(unused, but passed for consistency)
 *		arg1 (I)	int unused
 *		arg2 (I)	int unused
 *		arg3 (I)	int unused
 * --------------------------------------------------------------------------
 * Returns:	( subraster * )	ptr to subraster corresponding to ellipse
 *				requested, or NULL for any parsing error
 * --------------------------------------------------------------------------
 * Notes:     o	Summary of syntax...
 *		  \circle(xdiam[,ydiam])
 *	      o
 * ======================================================================= */
/* --- entry point --- */
subraster *rastcircle ( char **expression, int size, subraster *basesp,
			int arg1, int arg2, int arg3 )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
char	*texsubexpr(), circexpr[512],*xptr=circexpr; /*circle(xdiam[,ydiam])*/
char	*qptr=NULL, quads[256]="1234";	/* default to draw all quadrants */
double	theta0=0.0, theta1=0.0;		/* ;theta0,theta1 instead of ;quads*/
subraster *new_subraster(), *circsp=NULL; /* subraster for ellipse */
int	pixsz = 1;			/* pixels are one bit each */
double	xdiam=0.0, ydiam=0.0;		/* x,y major/minor axes/diameters */
int	width=0,  height=0,		/* #pixels width,height of ellipse */
	maxwidth=1600, maxheight=1600;	/* max width,height in pixels */
int	thickness = 1;			/* drawn lines are one pixel thick */
int	evalterm();			/* evaluate [arg],{arg} expressions*/
int	origin = 55;			/* force origin centered */
int	circle_raster(),		/* draw ellipse in circsp->image */
	circle_recurse();		/* for theta0,theta1 args */
/* -------------------------------------------------------------------------
obtain (xdiam[,ydiam]) arguments immediately following \circle command
-------------------------------------------------------------------------- */
/* --- parse for (xdiam[,ydiam]) args, and bump expression past it --- */
*expression = texsubexpr(*expression,circexpr,500,"(",")",0,0);
if ( *circexpr == '\000' ) goto end_of_job; /* couldn't get (xdiam[,ydiam])*/
/* --- now interpret xdiam[,ydiam] returned in circexpr --- */
if ( (qptr=strchr(circexpr,';')) != NULL ) /* semicolon signals quads data */
  { *qptr = '\000';			/* replace semicolon by '\0' */
    strninit(quads,qptr+1,128);		/* save user-requested quads */
    if ( (qptr=strchr(quads,',')) != NULL ) /* have theta0,theta1 instead */
      {	*qptr = '\000';			/* replace , with null */
	theta0 = (double)evalterm(mimestore,quads);  /* theta0 precedes , */
	theta1 = (double)evalterm(mimestore,qptr+1); /* theta1 follows , */
	qptr = NULL; }			/* signal thetas instead of quads */
    else
	qptr = quads; }			/* set qptr arg for circle_raster()*/
else					/* no ;quads at all */
  qptr = quads;				/* default to all 4 quadrants */
if ( (xptr=strchr(circexpr,',')) != NULL ) /* look for ',' in xdiam[,ydiam]*/
  *xptr = '\000';			/* found it, so replace ',' by '\0'*/
xdiam = ydiam =				/* xdiam=ydiam in user units */
  (double)evalterm(mimestore,circexpr);	/* evaluate expression */
if ( xptr != NULL )			/* 2nd arg, if present, is ydiam */
  ydiam = (double)evalterm(mimestore,xptr+1); /* in user units */
/* -------------------------------------------------------------------------
calculate width,height, etc
-------------------------------------------------------------------------- */
/* --- calculate width,height in pixels --- */
width  = max2(1,iround(unitlength*xdiam)); /*scale by unitlength and round,*/
height = max2(1,iround(unitlength*ydiam)); /* and must be at least 1 pixel */
if ( msgfp!=NULL && msglevel>=29 )	/* debugging */
  fprintf(msgfp,"rastcircle> width,height;quads=%d,%d,%s\n",
  width,height,(qptr==NULL?"default":qptr));
/* -------------------------------------------------------------------------
allocate subraster and raster for complete picture
-------------------------------------------------------------------------- */
/* --- sanity check on width,height args --- */
if ( width < 1 ||  width > maxwidth
||  height < 1 || height > maxheight ) goto end_of_job;
/* --- allocate and initialize subraster for constructed ellipse --- */
if ( (circsp=new_subraster(width,height,pixsz)) /* allocate new subraster */
==   NULL )  goto end_of_job;		/* quit if failed */
/* --- initialize ellipse subraster parameters --- */
circsp->type = IMAGERASTER;		/* image */
circsp->symdef = NULL;			/* not applicable for image */
circsp->baseline = height/2 + 2;	/* is a little above center good? */
circsp->size = size;			/* size (probably unneeded) */
/* -------------------------------------------------------------------------
draw the ellipse
-------------------------------------------------------------------------- */
if ( qptr != NULL )			/* have quads */
  circle_raster ( circsp->image,	/* embedded raster image */
	0, 0,				/* row0,col0 are upper-left corner */
	height-1, width-1,		/* row1,col1 are lower-right */
	thickness,			/* line thickness is 1 pixel */
	qptr );				/* "1234" quadrants to be drawn */
else					/* have theta0,theta1 */
  circle_recurse ( circsp->image,	/* embedded raster image */
	0, 0,				/* row0,col0 are upper-left corner */
	height-1, width-1,		/* row1,col1 are lower-right */
	thickness,			/* line thickness is 1 pixel */
	theta0,theta1 );		/* theta0,theta1 arc to be drawn */
/* -------------------------------------------------------------------------
return constructed ellipse to caller
-------------------------------------------------------------------------- */
end_of_job:
  if ( workingparam != NULL )		/* caller wants origin */
    *workingparam = origin;		/* return center origin to caller */
  return ( circsp );			/* return ellipse to caller */
} /* --- end-of-function rastcircle() --- */


/* ==========================================================================
 * Function:	rastbezier ( expression, size, basesp, arg1, arg2, arg3 )
 * Purpose:	\bezier handler, returns subraster corresponding to bezier
 *		parameters (col0,row0)(col1,row1)(colt,rowt)
 * --------------------------------------------------------------------------
 * Arguments:	expression (I/O) char **  to first char of null-terminated
 *				string immediately following \bezier to be
 *				rasterized, and returning ptr immediately
 *				following last character processed.
 *		size (I)	int containing 0-7 default font size
 *		basesp (I)	subraster *  to character (or subexpression)
 *				immediately preceding \bezier
 *				(unused, but passed for consistency)
 *		arg1 (I)	int unused
 *		arg2 (I)	int unused
 *		arg3 (I)	int unused
 * --------------------------------------------------------------------------
 * Returns:	( subraster * )	ptr to subraster corresponding to bezier
 *				requested, or NULL for any parsing error
 * --------------------------------------------------------------------------
 * Notes:     o	Summary of syntax...
 *		  \bezier(col1,row1)(colt,rowt)
 *	      o	col0=0,row0=0 assumed, i.e., given by
 *		\picture(){~(col0,row0){\bezier(col1,row1)(colt,rowt)}~}
 * ======================================================================= */
/* --- entry point --- */
subraster *rastbezier ( char **expression, int size, subraster *basesp,
			int arg1, int arg2, int arg3 )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
subraster *new_subraster(), *bezsp=NULL; /* subraster for bezier */
char	*texsubexpr(), bezexpr[129],*xptr=bezexpr; /*\bezier(r,c)(r,c)(r,c)*/
double	r0=0.0,c0=0.0, r1=0.0,c1=0.0, rt=0.0,ct=0.0, /* bezier points */
	rmid=0.0, cmid=0.0,		/* coords at parameterized midpoint*/
	rmin=0.0, cmin=0.0,		/* minimum r,c */
	rmax=0.0, cmax=0.0,		/* maximum r,c */
	rdelta=0.0, cdelta=0.0,		/* rmax-rmin, cmax-cmin */
	r=0.0, c=0.0;			/* some point */
int	evalterm();			/* evaluate [arg],{arg} expressions*/
int	iarg=0;				/* 0=r0,c0 1=r1,c1 2=rt,ct */
int	width=0, height=0,		/* dimensions of bezier raster */
	maxwidth=1600, maxheight=1600;	/* max width,height in pixels */
int	pixsz = 1;			/* pixels are one bit each */
/*int	thickness = 1;*/		/* drawn lines are one pixel thick */
int	origin = 0;			/*c's,r's reset to lower-left origin*/
int	bezier_raster();		/* draw bezier in bezsp->image */
/* -------------------------------------------------------------------------
obtain (c1,r1)(ct,rt) args immediately following \bezier command
-------------------------------------------------------------------------- */
for ( iarg=1; iarg<=2; iarg++ )		/* 0=c0,r0 1=c1,r1 2=ct,rt */
  {
  /* --- parse for (r,c) args, and bump expression past them all --- */
  *expression = texsubexpr(*expression,bezexpr,127,"(",")",0,0);
  if ( *bezexpr == '\000' ) goto end_of_job; /* couldn't get (r,c)*/
  /* --- now interpret (r,c) returned in bezexpr --- */
  c = r = 0.0;				/* init x-coord=col, y-coord=row */
  if ( (xptr=strchr(bezexpr,',')) != NULL ) /* comma separates row,col */
    { *xptr = '\000';			/* found it, so replace ',' by '\0'*/
      /* --- row=y-coord in pixels --- */
      r = unitlength*((double)evalterm(mimestore,xptr+1)); }
  /* --- col=x-coord in pixels --- */
  c = unitlength*((double)evalterm(mimestore,bezexpr));
  /* --- store r,c --- */
  switch ( iarg )
    { case 0: r0=r; c0=c; break;
      case 1: r1=r; c1=c; break;
      case 2: rt=r; ct=c; break; }
  } /* --- end-of-for(iarg) --- */
/* --- determine midpoint and maximum,minimum points --- */
rmid = 0.5*(rt + 0.5*(r0+r1));		/* y-coord at middle of bezier */
cmid = 0.5*(ct + 0.5*(c0+c1));		/* x-coord at middle of bezier */
rmin = min3(r0,r1,rmid);		/* lowest row */
cmin = min3(c0,c1,cmid);		/* leftmost col */
rmax = max3(r0,r1,rmid);		/* highest row */
cmax = max3(c0,c1,cmid);		/* rightmost col */
rdelta = rmax-rmin;			/* height */
cdelta = cmax-cmin;			/* width */
/* --- rescale coords so we start at 0,0 --- */
r0 -= rmin;  c0 -= cmin;		/* rescale r0,c0 */
r1 -= rmin;  c1 -= cmin;		/* rescale r1,c1 */
rt -= rmin;  ct -= cmin;		/* rescale rt,ct */
/* --- flip rows so 0,0 becomes lower-left corner instead of upper-left--- */
r0 = rdelta - r0 + 1;			/* map 0-->height-1, height-1-->0 */
r1 = rdelta - r1 + 1;
rt = rdelta - rt + 1;
/* --- determine width,height of raster needed for bezier --- */
width  = (int)(cdelta + 0.9999) + 1;	/* round width up */
height = (int)(rdelta + 0.9999) + 1;	/* round height up */
if ( msgfp!=NULL && msglevel>=29 )	/* debugging */
  fprintf(msgfp,"rastbezier> width,height,origin=%d,%d,%d; c0,r0=%g,%g; "
  "c1,r1=%g,%g\n rmin,mid,max=%g,%g,%g; cmin,mid,max=%g,%g,%g\n",
  width,height,origin, c0,r0, c1,r1, rmin,rmid,rmax, cmin,cmid,cmax);
/* -------------------------------------------------------------------------
allocate raster
-------------------------------------------------------------------------- */
/* --- sanity check on width,height args --- */
if ( width < 1 ||  width > maxwidth
||  height < 1 || height > maxheight ) goto end_of_job;
/* --- allocate and initialize subraster for constructed bezier --- */
if ( (bezsp=new_subraster(width,height,pixsz)) /* allocate new subraster */
==   NULL )  goto end_of_job;		/* quit if failed */
/* --- initialize bezier subraster parameters --- */
bezsp->type = IMAGERASTER;		/* image */
bezsp->symdef = NULL;			/* not applicable for image */
bezsp->baseline = height/2 + 2;		/* is a little above center good? */
bezsp->size = size;			/* size (probably unneeded) */
/* -------------------------------------------------------------------------
draw the bezier
-------------------------------------------------------------------------- */
bezier_raster ( bezsp->image,		/* embedded raster image */
	r0, c0,				/* row0,col0 are lower-left corner */
	r1, c1,				/* row1,col1 are upper-right */
	rt, ct );			/* bezier tangent point */
/* -------------------------------------------------------------------------
return constructed bezier to caller
-------------------------------------------------------------------------- */
end_of_job:
  if ( workingparam != NULL )		/* caller wants origin */
    *workingparam = origin;		/* return center origin to caller */
  return ( bezsp );			/* return bezier to caller */
} /* --- end-of-function rastbezier() --- */


/* ==========================================================================
 * Function:	rastraise ( expression, size, basesp, arg1, arg2, arg3 )
 * Purpose:	\raisebox{lift}{subexpression} handler, returns subraster
 *		containing subexpression with its baseline "lifted" by lift
 *		pixels, scaled by \unitlength, or "lowered" if lift arg
 *		negative
 * --------------------------------------------------------------------------
 * Arguments:	expression (I/O) char **  to first char of null-terminated
 *				string immediately following \raisebox to be
 *				rasterized, and returning ptr immediately
 *				following last character processed.
 *		size (I)	int containing 0-7 default font size
 *		basesp (I)	subraster *  to character (or subexpression)
 *				immediately preceding \raisebox
 *				(unused, but passed for consistency)
 *		arg1 (I)	int unused
 *		arg2 (I)	int unused
 *		arg3 (I)	int unused
 * --------------------------------------------------------------------------
 * Returns:	( subraster * )	ptr to subraster corresponding to \raisebox
 *				requested, or NULL for any parsing error
 * --------------------------------------------------------------------------
 * Notes:     o	Summary of syntax...
 *		  \raisebox{lift}{subexpression}
 *	      o
 * ======================================================================= */
/* --- entry point --- */
subraster *rastraise ( char **expression, int size, subraster *basesp,
			int arg1, int arg2, int arg3 )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
char	*texsubexpr(), subexpr[MAXSUBXSZ+1], *liftexpr=subexpr; /* args */
subraster *rasterize(), *raisesp=NULL;	/* rasterize subexpr to be raised */
int	lift=0;				/* amount to raise/lower baseline */
int	evalterm();			/* evaluate [arg],{arg} expressions*/
/* -------------------------------------------------------------------------
obtain {lift} argument immediately following \raisebox command
-------------------------------------------------------------------------- */
rastlift = 0;				/* reset global lift adjustment */
/* --- parse for {lift} arg, and bump expression past it --- */
*expression = texsubexpr(*expression,liftexpr,0,"{","}",0,0);
if ( *liftexpr == '\000' ) goto end_of_job; /* couldn't get {lift} */
lift = eround(liftexpr);		/* {lift} to integer */
if ( abs(lift) > 200 ) lift=0;		/* sanity check */
/* -------------------------------------------------------------------------
obtain {subexpr} argument after {lift}, and rasterize it
-------------------------------------------------------------------------- */
/* --- parse for {subexpr} arg, and bump expression past it --- */
*expression = texsubexpr(*expression,subexpr,0,"{","}",0,0);
/* --- rasterize subexpression to be raised/lowered --- */
if ( (raisesp = rasterize(subexpr,size)) /* rasterize subexpression */
==   NULL ) goto end_of_job;		/* and quit if failed */
/* -------------------------------------------------------------------------
raise/lower baseline and return it to caller
-------------------------------------------------------------------------- */
/* --- raise/lower baseline --- */
raisesp->baseline += lift;		/* new baseline (no height checks) */
rastlift = lift;			/* set global to signal adjustment */
/* --- return raised subexpr to caller --- */
end_of_job:
  return ( raisesp );			/* return raised subexpr to caller */
} /* --- end-of-function rastraise() --- */


/* ==========================================================================
 * Function:	rastrotate ( expression, size, basesp, arg1, arg2, arg3 )
 * Purpose:	\rotatebox{degrees}{subexpression} handler, returns subraster
 *		containing subexpression rotated by degrees (counterclockwise
 *		if degrees positive)
 * --------------------------------------------------------------------------
 * Arguments:	expression (I/O) char **  to first char of null-terminated
 *				string immediately following \rotatebox to be
 *				rasterized, and returning ptr immediately
 *				following last character processed.
 *		size (I)	int containing 0-7 default font size
 *		basesp (I)	subraster *  to character (or subexpression)
 *				immediately preceding \rotatebox
 *				(unused, but passed for consistency)
 *		arg1 (I)	int unused
 *		arg2 (I)	int unused
 *		arg3 (I)	int unused
 * --------------------------------------------------------------------------
 * Returns:	( subraster * )	ptr to subraster corresponding to \rotatebox
 *				requested, or NULL for any parsing error
 * --------------------------------------------------------------------------
 * Notes:     o	Summary of syntax...
 *		  \rotatebox{degrees}{subexpression}
 *	      o
 * ======================================================================= */
/* --- entry point --- */
subraster *rastrotate ( char **expression, int size, subraster *basesp,
			int arg1, int arg2, int arg3 )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
char	*texsubexpr(), subexpr[MAXSUBXSZ+1], *degexpr=subexpr; /* args */
subraster *rasterize(), *rotsp=NULL;	/* subraster for rotated subexpr */
raster	*rastrot(), *rotrp=NULL;	/* rotate subraster->image 90 degs */
int	delete_raster();		/* delete intermediate rasters */
int	baseline=0;			/* baseline of rasterized image */
double	degrees=0.0, ipart,fpart;	/* degrees to be rotated */
int	idegrees=0, isneg=0;		/* positive ipart, isneg=1 if neg */
int	n90=0, isn90=1;			/* degrees is n90 multiples of 90 */
int	evalterm();			/* evaluate [arg],{arg} expressions*/
/* -------------------------------------------------------------------------
obtain {degrees} argument immediately following \rotatebox command
-------------------------------------------------------------------------- */
/* --- parse for {degrees} arg, and bump expression past it --- */
*expression = texsubexpr(*expression,degexpr,0,"{","}",0,0);
if ( *degexpr == '\000' ) goto end_of_job; /* couldn't get {degrees} */
degrees = (double)evalterm(mimestore,degexpr); /* degrees to be rotated */
if ( degrees < 0.0 )			/* clockwise rotation desired */
  { degrees = -degrees;			/* flip sign so degrees positive */
    isneg = 1; }			/* and set flag to indicate flip */
fpart = modf(degrees,&ipart);		/* integer and fractional parts */
ipart = (double)(((int)degrees)%360);	/* degrees mod 360 */
degrees = ipart + fpart;		/* restore fractional part */
if ( isneg )				/* if clockwise rotation requested */
  degrees = 360.0 - degrees;		/* do equivalent counterclockwise */
idegrees = (int)(degrees+0.5);		/* integer degrees */
n90 = idegrees/90;			/* degrees is n90 multiples of 90 */
isn90 = (90*n90==idegrees);		/*true if degrees is multiple of 90*/
isn90 = 1;				/* forced true for time being */
/* -------------------------------------------------------------------------
obtain {subexpr} argument after {degrees}, and rasterize it
-------------------------------------------------------------------------- */
/* --- parse for {subexpr} arg, and bump expression past it --- */
*expression = texsubexpr(*expression,subexpr,0,"{","}",0,0);
/* --- rasterize subexpression to be rotated --- */
if ( (rotsp = rasterize(subexpr,size))	/* rasterize subexpression */
==   NULL ) goto end_of_job;		/* and quit if failed */
/* --- return unmodified image if no rotation requested --- */
if ( abs(idegrees) < 2 ) goto end_of_job; /* don't bother rotating image */
/* --- extract params for image to be rotated --- */
rotrp = rotsp->image;			/* unrotated rasterized image */
baseline = rotsp->baseline;		/* and baseline of that image */
/* -------------------------------------------------------------------------
rotate by multiples of 90 degrees
-------------------------------------------------------------------------- */
if ( isn90 )				/* rotation by multiples of 90 */
 if ( n90 > 0 )				/* do nothing for 0 degrees */
  {
  n90 = 4-n90;				/* rasrot() rotates clockwise */
  while ( n90 > 0 )			/* still have remaining rotations */
    { raster *nextrp = rastrot(rotrp);	/* rotate raster image */
      if ( nextrp == NULL ) break;	/* something's terribly wrong */
      delete_raster(rotrp);		/* free previous raster image */
      rotrp = nextrp;			/* and replace it with rotated one */
      n90--; }				/* decrement remaining count */
  } /* --- end-of-if(isn90) --- */
/* -------------------------------------------------------------------------
requested rotation not multiple of 90 degrees
-------------------------------------------------------------------------- */
if ( !isn90 )				/* explicitly construct rotation */
  { ; }					/* not yet implemented */
/* -------------------------------------------------------------------------
re-populate subraster envelope with rotated image
-------------------------------------------------------------------------- */
/* --- re-init various subraster parameters, embedding raster in it --- */
if ( rotrp != NULL )			/* rotated raster constructed okay */
 { rotsp->type = IMAGERASTER;		/* signal constructed image */
   rotsp->image = rotrp;		/* raster we just constructed */
   /* --- now try to guess pleasing baseline --- */
   if ( idegrees > 2 ) {		/* leave unchanged if unrotated */
    if ( strlen(subexpr) < 3		/* we rotated a short expression */
    ||   abs(idegrees-180) < 3 )	/* or just turned it upside-down */
      baseline = rotrp->height - 1;	/* so set with nothing descending */
    else				/* rotated a long expression */
      baseline = (65*(rotrp->height-1))/100; } /* roughly center long expr */
   rotsp->baseline = baseline; }	/* set baseline as calculated above*/
/* --- return rotated subexpr to caller --- */
end_of_job:
  return ( rotsp );			/*return rotated subexpr to caller*/
} /* --- end-of-function rastrotate() --- */


/* ==========================================================================
 * Function:	rastmagnify ( expression, size, basesp, arg1, arg2, arg3 )
 * Purpose:	\magnify{magstep}{subexpression} handler, returns subraster
 *		containing magnified subexpression
 * --------------------------------------------------------------------------
 * Arguments:	expression (I/O) char **  to first char of null-terminated
 *				string immediately following \reflectbox to
 *				be rasterized, and returning ptr immediately
 *				following last character processed.
 *		size (I)	int containing 0-7 default font size
 *		basesp (I)	subraster *  to character (or subexpression)
 *				immediately preceding \reflectbox
 *				(unused, but passed for consistency)
 *		arg1 (I)	int unused
 *		arg2 (I)	int unused
 *		arg3 (I)	int unused
 * --------------------------------------------------------------------------
 * Returns:	( subraster * )	ptr to subraster corresponding to \magnify
 *				requested, or NULL for any parsing error
 * --------------------------------------------------------------------------
 * Notes:     o	Summary of syntax...
 *		  \magnify{magstep}{subexpression}
 *	      o
 * ======================================================================= */
/* --- entry point --- */
subraster *rastmagnify ( char **expression, int size, subraster *basesp,
			int arg1, int arg2, int arg3 )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
char	*texsubexpr(), subexpr[MAXSUBXSZ+1], *magexpr=subexpr; /* args */
subraster *rasterize(), *magsp=NULL;	/* subraster for magnified subexpr */
raster	*rastmag(), *magrp=NULL;	/* magnify subraster->image */
int	magstep = 1;			/* default magnification */
int	delete_raster();		/* delete intermediate raster */
int	baseline=0;			/* baseline of rasterized image */
/* -------------------------------------------------------------------------
obtain {magstep} argument immediately following \magnify command
-------------------------------------------------------------------------- */
/* --- parse for {magstep} arg, and bump expression past it --- */
*expression = texsubexpr(*expression,magexpr,255,"{","}",0,0);
magstep = atoi(magexpr);		/* convert {magstep} to int */
if ( magstep<1 || magstep>10 )		/* check magstep input */
  magstep = 1;				/* back to default if illegal */
/* -------------------------------------------------------------------------
obtain {subexpr} argument after {magstep}, and rasterize it
-------------------------------------------------------------------------- */
/* --- parse for {subexpr} arg, and bump expression past it --- */
*expression = texsubexpr(*expression,subexpr,0,"{","}",0,0);
/* --- rasterize subexpression to be reflected --- */
if ( (magsp = rasterize(subexpr,size))	/* rasterize subexpression */
==   NULL ) goto end_of_job;		/* and quit if failed */
/* --- return unmodified image if no magnification requested --- */
if ( magstep<=1 ) goto end_of_job;	/* don't bother magnifying image */
/* --- extract params for image to be magnified --- */
magrp = magsp->image;			/* unmagnified rasterized image */
baseline = magsp->baseline;		/* and baseline of that image */
/* -------------------------------------------------------------------------
magnify image and adjust its parameters
-------------------------------------------------------------------------- */
/* --- magnify image --- */
magrp = rastmag(magsp->image,magstep);	/* magnify raster image */
if ( magrp == NULL ) goto end_of_job;	/* failed to magnify image */
delete_raster(magsp->image);		/* free original raster image */
magsp->image = magrp;			/*and replace it with magnified one*/
/* --- adjust parameters --- */
baseline *= magstep;			/* scale baseline */
if ( baseline > 0 ) baseline += 1;	/* adjust for no descenders */
magsp->baseline = baseline;		/*reset baseline of magnified image*/
/* --- return magnified subexpr to caller --- */
end_of_job:
  return ( magsp );			/*back to caller with magnified expr*/
} /* --- end-of-function rastmagnify() --- */


/* ==========================================================================
 * Function:	rastreflect ( expression, size, basesp, arg1, arg2, arg3 )
 * Purpose:	\reflectbox[axis]{subexpression} handler, returns subraster
 *		containing subexpression reflected horizontally (i.e., around
 *		vertical axis, |_ becomes _|) if [axis] not given or axis=1,
 *		or reflected vertically if axis=2 given.
 * --------------------------------------------------------------------------
 * Arguments:	expression (I/O) char **  to first char of null-terminated
 *				string immediately following \reflectbox to
 *				be rasterized, and returning ptr immediately
 *				following last character processed.
 *		size (I)	int containing 0-7 default font size
 *		basesp (I)	subraster *  to character (or subexpression)
 *				immediately preceding \reflectbox
 *				(unused, but passed for consistency)
 *		arg1 (I)	int unused
 *		arg2 (I)	int unused
 *		arg3 (I)	int unused
 * --------------------------------------------------------------------------
 * Returns:	( subraster * )	ptr to subraster corresponding to \reflectbox
 *				requested, or NULL for any parsing error
 * --------------------------------------------------------------------------
 * Notes:     o	Summary of syntax...
 *		  \reflectbox[axis]{subexpression}
 *	      o
 * ======================================================================= */
/* --- entry point --- */
subraster *rastreflect ( char **expression, int size, subraster *basesp,
			int arg1, int arg2, int arg3 )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
char	*texsubexpr(), subexpr[MAXSUBXSZ+1], *axisexpr=subexpr; /* args */
subraster *rasterize(), *refsp=NULL;	/* subraster for reflected subexpr */
raster	*rastref(), *refrp=NULL;	/* reflect subraster->image */
int	axis = 1;			/* default horizontal reflection */
int	delete_raster();		/* delete intermediate raster */
int	baseline=0;			/* baseline of rasterized image */
/* -------------------------------------------------------------------------
obtain [axis] argument immediately following \reflectbox command, if given
-------------------------------------------------------------------------- */
/* --- check for optional [axis] arg  --- */
if ( *(*expression) == '[' )		/*check for []-enclosed optional arg*/
  { *expression = texsubexpr(*expression,axisexpr,255,"[","]",0,0);
    axis = atoi(axisexpr);		/* convert [axis] to int */
    if ( axis<1 || axis>2 )		/* check axis input */
      axis = 1; }			/* back to default if illegal */
/* -------------------------------------------------------------------------
obtain {subexpr} argument after optional [axis], and rasterize it
-------------------------------------------------------------------------- */
/* --- parse for {subexpr} arg, and bump expression past it --- */
*expression = texsubexpr(*expression,subexpr,0,"{","}",0,0);
/* --- rasterize subexpression to be reflected --- */
if ( (refsp = rasterize(subexpr,size))	/* rasterize subexpression */
==   NULL ) goto end_of_job;		/* and quit if failed */
/* --- return unmodified image if no reflection requested --- */
if ( axis<1 || axis>2 ) goto end_of_job; /* don't bother reflecting image */
/* --- extract params for image to be reflected --- */
refrp = refsp->image;			/* unreflected rasterized image */
baseline = refsp->baseline;		/* and baseline of that image */
/* -------------------------------------------------------------------------
reflect image and adjust its parameters
-------------------------------------------------------------------------- */
/* --- reflect image --- */
refrp = rastref(refsp->image,axis);	/* reflect raster image */
if ( refrp == NULL ) goto end_of_job;	/* failed to reflect image */
delete_raster(refsp->image);		/* free original raster image */
refsp->image = refrp;			/*and replace it with reflected one*/
/* --- adjust parameters --- */
if ( axis == 2 )			/* for vertical reflection */
  baseline = refrp->height - 1;		/* set with nothing descending */
refsp->baseline = baseline;		/* reset baseline of reflected image*/
/* --- return reflected subexpr to caller --- */
end_of_job:
  return ( refsp );			/*back to caller with reflected expr*/
} /* --- end-of-function rastreflect() --- */


/* ==========================================================================
 * Function:	rastfbox ( expression, size, basesp, arg1, arg2, arg3 )
 * Purpose:	\fbox{subexpression} handler, returns subraster
 *		containing subexpression with frame box drawn around it
 * --------------------------------------------------------------------------
 * Arguments:	expression (I/O) char **  to first char of null-terminated
 *				string immediately following \fbox to be
 *				rasterized, and returning ptr immediately
 *				following last character processed.
 *		size (I)	int containing 0-7 default font size
 *		basesp (I)	subraster *  to character (or subexpression)
 *				immediately preceding \fbox
 *				(unused, but passed for consistency)
 *		arg1 (I)	int unused
 *		arg2 (I)	int unused
 *		arg3 (I)	int unused
 * --------------------------------------------------------------------------
 * Returns:	( subraster * )	ptr to subraster corresponding to \fbox
 *				requested, or NULL for any parsing error
 * --------------------------------------------------------------------------
 * Notes:     o	Summary of syntax...
 *		  \fbox[width][height]{subexpression}
 *	      o
 * ======================================================================= */
/* --- entry point --- */
subraster *rastfbox ( char **expression, int size, subraster *basesp,
			int arg1, int arg2, int arg3 )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
char	*texsubexpr(), subexpr[MAXSUBXSZ+1], widtharg[512]; /* args */
subraster *rasterize(), *framesp=NULL;	/* rasterize subexpr to be framed */
raster	*border_raster(), *bp=NULL;	/* framed image raster */
int	evalterm(), evalue=0;		/* interpret [width][height] */
int	fwidth=6, fthick=1,		/*extra frame width, line thickness*/
	fsides=0;		/* frame sides: 1=left,2=top,4=right,8=bot */
int	width=(-1), height=(-1),	/* optional [width][height] args */
	iscompose = 0;			/* set true if optional args given */
/* -------------------------------------------------------------------------
obtain optional [width][height] arguments immediately following \fbox
-------------------------------------------------------------------------- */
/* --- first check for optional \fbox[width] --- */
if ( *(*expression) == '[' ) {		/* check for []-enclosed width arg */
  *expression = texsubexpr(*expression,widtharg,511,"[","]",0,0);
  if ( !isempty(widtharg) ) {		/* got widtharg */
    char *comma = strchr(widtharg,',');	/* look for [width,sides] */
    if ( comma == (char *)NULL )	/* no comma */
      comma = strchr(widtharg,';');	/* permit semicolon [width;sides] */
    if ( comma != (char *)NULL ) {	/* optional [width,fsides] found */
      fsides = atoi(comma+1);		/* interpret fsides after comma */
      if ( size < 5 )			/* for smaller fonts */
        { fwidth = 2;  fthick = 1; }	/* tighten frame, thinner accent */
      else { fwidth = 3;  fthick = 2; }	/* loosen frame, thicken accent */
      *comma = '\000';			/* null-terminate width at comma */
      trimwhite(widtharg); }		/*remove leading/trailing whitespace*/
    if ( comma==(char *)NULL || !isempty(widtharg) ) { /* have a width */
      height = 1;			/* default explicit height, too */
      if ( fsides == 0 ) {		/* a normal framebox */
	evalue = eround(widtharg);	/* interpret and scale width */
        width = max2(1,evalue);		/* must be >0 */
        fwidth = 2; iscompose = 1; }
      else				/* absolute pixels for "accents" */
	width = evalterm(mimestore,widtharg); }
    } /* --- end-of-if(!isempty(widtharg)) --- */
  } /* --- end-of-if(**expression=='[') --- */
if ( width > 0 || fsides > 0)		/* found leading [width], so... */
 if ( *(*expression) == '[' )		/* check for []-enclosed height arg */
  { *expression = texsubexpr(*expression,widtharg,511,"[","]",0,0);
    if ( !isempty(widtharg) ) {		/* got widtharg */
      if ( fsides == 0 ) {		/* a normal framebox */
	evalue = eround(widtharg);	/* interpret and scale height */
        height = max2(1,evalue);	/* must be >0 */
        fwidth = 0; }			/* no extra border */
      else				/* absolute pixels for "accents" */
	height = evalterm(mimestore,widtharg); }
  } /* --- end-of-if(**expression=='[') --- */
/* -------------------------------------------------------------------------
obtain {subexpr} argument
-------------------------------------------------------------------------- */
/* --- parse for {subexpr} arg, and bump expression past it --- */
*expression = texsubexpr(*expression,subexpr,0,"{","}",0,0);
/* --- rasterize subexpression to be framed --- */
if ( width<0 || height<0 )		/* no explicit dimensions given */
  { if ( (framesp = rasterize(subexpr,size)) /* rasterize subexpression */
    ==   NULL ) goto end_of_job; }	/* and quit if failed */
else
  { char composexpr[8192];		/* compose subexpr with empty box */
    sprintf(composexpr,"\\compose{\\hspace{%d}\\vspace{%d}}{%.8000s}",
    width,height,subexpr);
    if ( (framesp = rasterize(composexpr,size)) /* rasterize subexpression */
    ==   NULL ) goto end_of_job; }	/* and quit if failed */
/* -------------------------------------------------------------------------
draw frame, reset params, and return it to caller
-------------------------------------------------------------------------- */
/* --- draw border --- */
if ( fsides > 0 ) fthick += (100*fsides); /* embed fsides in fthick arg */
if ( (bp = border_raster(framesp->image,-fwidth,-fwidth,fthick,1))
==   NULL ) goto end_of_job;		/* draw border and quit if failed */
/* --- replace original image and raise baseline to accommodate frame --- */
framesp->image = bp;			/* replace image with framed one */
if ( !iscompose )			/* simple border around subexpr */
  framesp->baseline += fwidth;		/* so just raise baseline */
else
  framesp->baseline = (framesp->image)->height - 1; /* set at bottom */
/* --- return framed subexpr to caller --- */
end_of_job:
  return ( framesp );			/* return framed subexpr to caller */
} /* --- end-of-function rastfbox() --- */


/* ==========================================================================
 * Function:	rastovalbox ( expression, size, basesp, arg1, arg2, arg3 )
 * Purpose:	\ovalbox{subexpression} handler, returns subraster
 *		containing subexpression with ellipse drawn around it
 * --------------------------------------------------------------------------
 * Arguments:	expression (I/O) char **  to first char of null-terminated
 *				string immediately following \ovalbox to be
 *				rasterized, and returning ptr immediately
 *				following last character processed.
 *		size (I)	int containing 0-7 default font size
 *		basesp (I)	subraster *  to character (or subexpression)
 *				immediately preceding \ovalbox
 *				(unused, but passed for consistency)
 *		arg1 (I)	int unused
 *		arg2 (I)	int unused
 *		arg3 (I)	int unused
 * --------------------------------------------------------------------------
 * Returns:	( subraster * )	ptr to subraster corresponding to \ovalbox
 *				requested, or NULL for any parsing error
 * --------------------------------------------------------------------------
 * Notes:     o	Summary of syntax...
 *		  \ovalbox[n;#ovals,wdelta,hdelta]{subexpression}
 *	      o	Originally copied from rastfbox(), above,
 *		but I've modified the [] optional argument parsing
 *	      o Thanks to answers from Jean-Marie Becker and "Narasimham" at
 *		   http://math.stackexchange.com/questions/2149677/
 *		for providing info about ellipse circumscribing a rectangle
 * ======================================================================= */
/* --- entry point --- */
subraster *rastovalbox ( char **expression, int size, subraster *basesp,
			int arg1, int arg2, int arg3 )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
char	*texsubexpr(), subexpr[MAXSUBXSZ+1], narg[512], /* args */
	composexpr[MAXSUBXSZ+2048],	/* compose subexpr[] with ellipse(s)*/
	ovalexpr[2048];			/* nested \compose{}{}'s */
subraster *rasterize(), *framesp=NULL;	/* rasterize subexpr to be framed */
int	delete_subraster();		/* just need width,height */
int	fwidth = 3+size/999;		/* extra frame width */
int	width=(-1), height=(-1),	/* width,height of subexpr[] */
	origheight = (-1),		/* original subexpr[] height */
	baseline = (-1);		/* and its original baseline */
/*double sqrt2 = 1.414213562;*/		/*you think that's accurate enough?*/
double	x0=0.0, y0=0.0,			/*rectangle half-width, half-height*/
	a=0.0,  b=0.0;			/*ellipse semi-major, semi-minor axis*/
double	n = 2.0;			/*width/height=(semi-major/minor)^n*/
int	ioval=0, novals=1,		/* #concentric ovals to draw */
	wdelta=(3), hdelta=(-3);	/*width,height deltas for each oval*/
/* -------------------------------------------------------------------------
obtain {subexpr} argument
-------------------------------------------------------------------------- */
/* --- first check for optional \ovalbox[n] and bump expression past it--- */
if ( *(*expression) == '[' ) {		/* check for []-enclosed n arg */
  *expression = texsubexpr(*expression,narg,511,"[","]",0,0);
  if ( !isempty(narg) ) {		/* got n */
    char *semi = strchr(narg,';');	/* optionally followed by ;#ovals */
    if ( semi != NULL ) {		/* found optional ;#ovals */
      char *comma = strchr(semi+1,',');	/* optionally followed by ,w,hdelta */
      *semi = '\000';			/* null-terminate 'n' argument at ; */
      if ( comma != NULL ) *comma = '\000'; /* and #ovals arg at (first) , */
      if ( (novals=atoi(semi+1))	/* convert #ovals argument after ; */
      < 1 ) novals = 1;			/* and make sure it's >0 */
      if ( comma != NULL ) {		/*have ,wdelta and maybe ,hdelta args*/
        char *comma2 = strchr(comma+1,','); /* ,wdelta and maybe ,hdelta */
        if ( comma2 != NULL ) *comma2 = '\000'; /* null-terminate wdelta */
        hdelta = wdelta = atoi(comma+1); /* init both wdelta,hdelta */
        if ( comma2 != NULL )		/* but we have separate ,hdelta */
          hdelta = atoi(comma2+1);	/* so set hdelta separately */
        } /* --- end-of-if(comma!=NULL) --- */
      } /* --- end-of-if(semi!=NULL) --- */
    if ( (n = atof(narg))		/* convert narg to double */
    <=   .001 ) n = 2.0;		/* sanity check (revert to default) */
    } /* --- end-of-if(!isempty(narg)) --- */
  } /* --- end-of-if(**expression=='[') --- */
/* --- parse for {subexpr} arg, and bump expression past it --- */
*expression = texsubexpr(*expression,subexpr,0,"{","}",0,0);
/* -------------------------------------------------------------------------
rasterize subexpression to be ellipse-framed
-------------------------------------------------------------------------- */
if ( (framesp = rasterize(subexpr,size)) /* rasterize subexpression */
==   NULL ) goto end_of_job;		/* and quit if failed */
if ( framesp->image == NULL ) goto end_of_job; /* or quit if no image */
/* --- pull out info we need --- */
width  = (framesp->image)->width;	/* width  of subexpr image */
height = (framesp->image)->height;	/* height of subexpr image */
baseline = framesp->baseline;		/* and original baseline */
delete_subraster(framesp);		/* just needed width,height,baseline */
origheight = height;			/* original "rectangle" height */
/* --- old formulas for circumscribing ellipse (corresponds to n=1) --- */
/*width  = (int)(0.5+sqrt2*(double)(width +fwidth));*/ /*major ellipse axis*/
/*height = (int)(0.5+sqrt2*(double)(height+fwidth));*/ /*minor ellipse axis*/
/* --- use general formulas for circumscribing ellipse --- */
x0 = ((double)width)/2.0;  y0 = ((double)height)/2.0; /* half-width,height */
a = sqrt(x0*x0 + pow(x0,2.0/n)*pow(y0,2.0-2.0/n));
b = sqrt(y0*y0 + pow(y0,2.0/n)*pow(x0,2.0-2.0/n));
width  = (int)(2.0*a + 0.5) + fwidth;
height = (int)(2.0*b + 0.5) + fwidth;
baseline += ((height-origheight)+1)/2;	/* baseline of subexpr[] chars */
/* -------------------------------------------------------------------------
construct nested \compose{\compose{\circle(,)}{\circle(,)}}{\circle(,)}
-------------------------------------------------------------------------- */
*ovalexpr = '\000';			/* init as empty string */
for ( ioval=0; ioval<novals; ioval++ ) { /* nest each oval */
  char thisoval[2048],			/* oval to be nested with ovalexpr */
       ovalbuff[2048];			/* temp ovalexpr buffer */
  int thiswidth  =  width + (ioval*wdelta), /*  width for this oval */
      thisheight = height + (ioval*hdelta); /* height for this oval */
  if ( strlen(ovalexpr) > 1024 ) break;	/* sanity check */
  strcpy(ovalbuff,ovalexpr);		/* save current ovalexpr */
  sprintf(thisoval,"\\circle(%d,%d)",thiswidth,thisheight); /*draw this oval*/
  if ( ioval == 0 ) {			/* initial oval */
    strcpy(ovalexpr,thisoval); }	/* just copy \circle(,) directive */
  else {				/* nest with \compose{}{}'s */
    sprintf(ovalexpr,"\\compose{%s}{%s}",ovalbuff,thisoval); }
  } /* --- end-of-for(ioval) --- */
/* -------------------------------------------------------------------------
compose with circumscribed ellipse(s), reset params, and return it to caller
-------------------------------------------------------------------------- */
/* --- construct expression to be rasterized
       (note: too bad we have to re-rasterize original subexpr[]) --- */
      /* sprintf(composexpr,"\\compose{\\circle(%d,%d)}{%.8000s}",
         width,height,subexpr); */
sprintf(composexpr,"\\compose{%s}{%.8000s}", ovalexpr,subexpr);
if ( (framesp = rasterize(composexpr,size)) /*rasterize ellipse with subexpr*/
==   NULL ) goto end_of_job;		/* and quit if failed */
framesp->baseline = baseline;		/* reset baseline (I hope) */
/* --- return ellipse-framed subexpr to caller --- */
end_of_job:
  return ( framesp );			/* return framed subexpr to caller */
} /* --- end-of-function rastovalbox() --- */


/* ==========================================================================
 * Function:	rastinput ( expression, size, basesp, arg1, arg2, arg3 )
 * Purpose:	\input{filename} handler, reads filename and returns
 *		subraster containing image of expression read from filename
 * --------------------------------------------------------------------------
 * Arguments:	expression (I/O) char **  to first char of null-terminated
 *				string immediately following \input to be
 *				rasterized, and returning ptr immediately
 *				following last character processed.
 *		size (I)	int containing 0-7 default font size
 *		basesp (I)	subraster *  to character (or subexpression)
 *				immediately preceding \input
 *				(unused, but passed for consistency)
 *		arg1 (I)	int unused
 *		arg2 (I)	int unused
 *		arg3 (I)	int unused
 * --------------------------------------------------------------------------
 * Returns:	( subraster * )	ptr to subraster corresponding to expression
 *				in filename, or NULL for any parsing error
 * --------------------------------------------------------------------------
 * Notes:     o	Summary of syntax...
 *		  \input{filename}     reads entire file named filename
 *		  \input{filename:tag} reads filename, but returns only
 *		  those characters between <tag>...</tag> in that file.
 *	      o
 * ======================================================================= */
/* --- entry point --- */
subraster *rastinput ( char **expression, int size, subraster *basesp,
			int arg1, int arg2, int arg3 )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
char	*texsubexpr(), tag[1024]="\000", filename[1024]="\000"; /* args */
subraster *rasterize(), *inputsp=NULL, /* rasterized input image */
	*read_pbm();		/* read .pbm file and make subraster image */
int	status, rastreadfile();	/* read input file */
FILE	*fp=NULL, *rastopenfile(); /* reading .pbm files locally */
int	format=0, npts=0,	/* don't reformat (numerical) input */
	ispbm = 0;		/* set true for \input[pbm]{filename.pbm} */
double	sf = 0.0;		/* shrink factor (0 or 1 means don't shrink)*/
int	isinput = (seclevel<=inputseclevel?1:0); /*true if \input permitted*/
/*int	evalterm();*/		/* evaluate expressions */
char	*inputpath = INPUTPATH;	/* permitted \input{} paths for any user */
int	isstrstr();		/* search for valid inputpath in filename */
char	subexpr[MAXFILESZ+1] = "\000", /*concatanated lines from input file*/
	*mimeprep(),		/* preprocess inputted data */
	*dbltoa(), *reformat=NULL; /* reformat numerical input */
/* -------------------------------------------------------------------------
obtain [tag]{filename} argument
-------------------------------------------------------------------------- */
/* --- parse for optional [tag] or [fmt] arg, bump expression past it --- */
if ( *(*expression) == '[' )		/* check for []-enclosed value */
  { char argfld[MAXTOKNSZ+1];		/* optional argument field */
    *expression = texsubexpr(*expression,argfld,MAXTOKNSZ-1,"[","]",0,0);
    if ( (reformat=strstr(argfld,"dtoa")) != NULL ) /*dtoa/dbltoa requested*/
      {	format = 1;			/* signal dtoa()/dbltoa() format */
	if ( (reformat=strchr(reformat,'=')) != NULL ) /* have dtoa= */
	  npts = (int)strtol(reformat+1,NULL,0); } /* so set npts */
    if ( (reformat=strstr(argfld,"pbm")) != NULL ) /*input a .pbm image file*/
      { ispbm = 1;			/* set ispbm flag */
	if ( (reformat=strchr(reformat,',')) != NULL ) /* have pbm, */
	  sf = strtod(reformat+1,NULL); } /* so set sf(shrinkfactor) */
    if ( format==0 && !ispbm ) {	/* not reformat request, and not pbm */
      strninit(tag,argfld,1020); } }	/* so interpret arg as tag */
/* --- parse for {filename} arg, and bump expression past it --- */
*expression = texsubexpr(*expression,filename,1020,"{","}",0,0);
/* --- check for alternate filename:tag --- */
if ( !isempty(filename)			/* got filename */
/*&& isempty(tag)*/ )			/* but no [tag] */
 { char	*delim = strchr(filename,':');	/* look for : in filename:tag */
   if ( delim != (char *)NULL )		/* found it */
    { *delim = '\000';			/* null-terminate filename at : */
      strninit(tag,delim+1,1020); } }	/* and stuff after : is tag */
/* --- check filename for an inputpath valid for all users --- */
if ( !isinput				/* if this user can't \input{} */
&&   !isempty(filename)			/* and we got a filename */
&&   !isempty(inputpath) )		/* and an inputpath */
  if ( isstrstr(filename,inputpath,0) )	/* filename has allowed inputpath */
    isinput = 1;			/* okay to \input{} this filename */
/* --- guard against recursive runaway (e.g., file \input's itself) --- */
if ( ++ninputcmds > 8 )			/* max \input's per expression */
  isinput = 0;				/* flip flag off after the max */
/* --------------------------------------------------------------------------
Read file (and convert to numeric if [dtoa] option was given)
-------------------------------------------------------------------------- */
if ( isinput ) {			/* user permitted to use \input{} */
  if ( ispbm ) {			/* special case: input a .pbm file */
    fp = rastopenfile(filename,"r");	/* open the .pbm file for "r"ead */
    if ( fp != NULL ) {			/* opened .pbm file successfully */
      if(0) printf("rastinput> sf=%.3f to read_pbm(%s,sf)\n",sf,filename);
      inputsp = read_pbm(fp,sf);	/* create subraster image from .pbm */
      fclose(fp); }			/* close file after reading */
    goto end_of_job;			/* all done, return inputsp to caller*/
    } /* --- end-of-if(ispbm) --- */
  status = rastreadfile(filename,0,tag,subexpr); /* read file */
  if ( *subexpr == '\000' ) goto end_of_job;   /* quit if problem */
  /* --- rasterize input subexpression  --- */
  mimeprep(subexpr);			/* preprocess subexpression */
  if ( format == 1 ) {			/* dtoa()/dbltoa() */
    double d = strtod(subexpr,NULL);	/* interpret subexpr as double */
    if ( d != 0.0 )			/* conversion to double successful */
      if ( (reformat=dbltoa(d,npts)) != NULL ) /* reformat successful */
        strcpy(subexpr,reformat); }	/*replace subexpr with reformatted*/
  } /* --- end-of-if(isinput) --- */
/* --------------------------------------------------------------------------
emit error message for unauthorized users trying to use \input{}
-------------------------------------------------------------------------- */
else {					/* inputseclevel > seclevel */
  sprintf(subexpr,
  "\\ \\text{[\\backslash input\\lbrace %.128s\\rbrace\\ not permitted]}\\ ",
  (isempty(filename)?"???":filename));
  } /* --- end-of-if/else(isinput) --- */
/* --------------------------------------------------------------------------
Rasterize constructed subexpression
-------------------------------------------------------------------------- */
inputsp = rasterize(subexpr,size);	/* rasterize subexpression */
/* --- return input image to caller --- */
end_of_job:
  return ( inputsp );			/* return input image to caller */
} /* --- end-of-function rastinput() --- */


/* ==========================================================================
 * Function:	rastcounter ( expression, size, basesp, arg1, arg2, arg3 )
 * Purpose:	\counter[value]{filename} handler, returns subraster
 *		containing image of counter value read from filename
 *		(or optional [value]), and increments counter
 * --------------------------------------------------------------------------
 * Arguments:	expression (I/O) char **  to first char of null-terminated
 *				string immediately following \counter to be
 *				rasterized, and returning ptr immediately
 *				following last character processed.
 *		size (I)	int containing 0-7 default font size
 *		basesp (I)	subraster *  to character (or subexpression)
 *				immediately preceding \counter
 *				(unused, but passed for consistency)
 *		arg1 (I)	int unused
 *		arg2 (I)	int unused
 *		arg3 (I)	int unused
 * --------------------------------------------------------------------------
 * Returns:	( subraster * )	ptr to subraster corresponding to \counter
 *				requested, or NULL for any parsing error
 * --------------------------------------------------------------------------
 * Notes:     o	Summary of syntax...
 *		  \counter[value][logfile]{filename:tag}
 *	      o	:tag is optional
 * ======================================================================= */
/* --- entry point --- */
subraster *rastcounter ( char **expression, int size, subraster *basesp,
			int arg1, int arg2, int arg3 )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
char	*texsubexpr(), filename[1024]="\000", /* counter file */
	logfile[1024]="\000", tag[1024]="\000"; /*optional log file and tag*/
subraster *rasterize(), *countersp=NULL; /* rasterized counter image */
FILE	/* *fp=NULL,*/ *logfp=NULL; /* counter and log file pointers */
int	status=0,rastreadfile(),rastwritefile(), /*read,write counter file*/
	iscounter = (seclevel<=counterseclevel?1:0), /*is \counter permitted*/
	isstrict = 1;		/* true to only write to existing files */
char	text[MAXFILESZ] = "1_",	/* only line in counter file without tags */
	*delim = NULL,		/* delimiter in text */
	utext[128] = "1_",	/* default delimiter */
	*udelim = utext+1;	/* underscore delimiter */
char	*rasteditfilename(),	/* edit log file name */
	*timestamp(),		/* timestamp for logging */
	*dbltoa();		/* double to comma-separated ascii */
int	counter = 1,		/* atoi(text) (after _ removed, if present) */
	value = 1,		/* optional [value] argument */
	gotvalue = 0,		/* set true if [value] supplied */
	isdelta = 0,		/* set true if [+value] or [-value] is delta*/
	ordindex = (-1);	/* ordinal[] index to append ordinal suffix */
/*--- ordinal suffixes based on units digit of counter ---*/
static	char *ordinal[]={"th","st","nd","rd","th","th","th","th","th","th"};
static	char *logvars[]={"REMOTE_ADDR","HTTP_REFERER",NULL}; /* log vars*/
static	int  commentvar = 1;	/* logvars[commentvar] replaced by comment */
/* -------------------------------------------------------------------------
first obtain optional [value][logfile] args immediately following \counter
-------------------------------------------------------------------------- */
/* --- first check for optional \counter[value] --- */
if ( *(*expression) == '[' )		/* check for []-enclosed value */
  { *expression = texsubexpr(*expression,text,1023,"[","]",0,0);
    if ( *text != '\000' )		/* got counter value (or logfile) */
     if ( strlen(text) >= 1 ) {		/* and it's not an empty string */
      if ( isthischar(*text,"+-0123456789") ) /* check for leading +-digit */
	gotvalue = 1;			/* signal we got optional value */
      else				/* not +-digit, so must be logfile */
	strcpy(logfile,text); }		/* so just copy it */
  } /* --- end-of-if(**expression=='[') --- */
/* --- next check for optional \counter[][logfile] --- */
if ( *(*expression) == '[' )		/* check for []-enclosed logfile */
  { *expression = texsubexpr(*expression,filename,1023,"[","]",0,0);
    if ( *filename != '\000' )		/* got logfile (or counter value) */
     if ( strlen(filename) >= 1 ) {	/* and it's not an empty string */
      if ( !(isthischar(*text,"+-0123456789")) /* not a leading +-digit */
      ||   gotvalue )			/* or we already got counter value */
	strcpy(logfile,filename);	/* so just copy it */
      else				/* leading +-digit must be value */
	{ strcpy(text,filename);	/* copy value to text line */
	  gotvalue = 1; } }		/* and signal we got optional value*/
  } /* --- end-of-if(**expression=='[') --- */
/* --- evaluate [value] if present --- */
if ( gotvalue ) {			/*leading +-digit should be in text*/
 if ( *text == '+' ) isdelta = (+1);	/* signal adding */
 if ( *text == '-' ) isdelta = (-1);	/* signal subtracting */
 value = (int)(strtod((isdelta==0?text:text+1),&udelim)+0.1); /*abs(value)*/
 if ( isdelta == (-1) ) value = (-value); /* set negative value if needed */
 counter = value;			/* re-init counter */
 } /* --- end-of-if(gotvalue) --- */
/* -------------------------------------------------------------------------
obtain counter {filename} argument
-------------------------------------------------------------------------- */
/* --- parse for {filename} arg, and bump expression past it --- */
*expression = texsubexpr(*expression,filename,1023,"{","}",0,0);
/* --- check for counter filename:tag --- */
if ( *filename != '\000' )		/* got filename */
 if ( (delim=strchr(filename,':'))	/* look for : in filename:tag */
 !=   (char *)NULL )			/* found it */
  { *delim = '\000';			/* null-terminate filename at : */
    strcpy(tag,delim+1); }		/* and stuff after : is tag */
/* --------------------------------------------------------------------------
emit error message for unauthorized users trying to use \counter{}
-------------------------------------------------------------------------- */
if ( !iscounter ) {			/* counterseclevel > seclevel */
 sprintf(text,
 "\\ \\text{[\\backslash counter\\lbrace %.128s\\rbrace\\ not permitted]}\\ ",
 (isempty(filename)?"???":filename));
 goto rasterize_counter;		/* rasterize error message */
 } /* --- end-of-if(!iscounter) --- */
/* --------------------------------------------------------------------------
Read and parse file, increment and rewrite counter (with optional underscore)
-------------------------------------------------------------------------- */
if ( strlen(filename) > 1 )		/* make sure we got {filename} arg */
  {
  /* --- read and interpret first (and only) line from counter file --- */
  if ( !gotvalue || (isdelta!=0) )	/*if no [count] arg or if delta arg*/
   if ( (status=rastreadfile(filename,1,tag,text)) > 0 ) /*try reading file*/
    { char *vdelim = NULL;		/* underscore delim from file */
      double fileval  = strtod(text,&vdelim); /* value and delim from file */
      counter = (int)(fileval<0.0?fileval-0.1:fileval+0.1); /* integerized */
      counter += value;			/* bump count by 1 or add/sub delta*/
      if ( !gotvalue ) udelim=vdelim; }	/* default to file's current delim */
  /* --- check for ordinal suffix --- */
  if ( udelim != (char *)NULL )		/* have some delim after value */
   if ( *udelim == '_' )		/* underscore signals ordinal */
    { int abscount = (counter>=0?counter:(-counter)); /* abs(counter) */
      ordindex = abscount%10;		/* least significant digit */
      if ( abscount >= 10 )		/* counter is 10 or greater */
       if ( (abscount/10)%10 == 1 )	/* and the last two are 10-19 */
	ordindex = 0; }		/* use th for 11,12,13 rather than st,nd,rd */
  /* --- rewrite counter file --- */
  if ( status >= 0 )			/* file was read okay */
   { sprintf(text,"%d",counter);	/*build image of incremented counter*/
     if ( ordindex >= 0 ) strcat(text,"_"); /* tack on _ */
     if ( *tag == '\000' ) strcat(text,"\n"); /* and newline */
     status = rastwritefile(filename,tag,text,isstrict); } /*rewrite counter*/
  } /* --- end-of-if(strlen(filename)>1) --- */
/* --------------------------------------------------------------------------
log counter request
-------------------------------------------------------------------------- */
if ( strlen(logfile) > 1 )		/* optional [logfile] given */
 {
 char	comment[1024] = "\000",		/* embedded comment, logfile:comment*/
	*commptr = strchr(logfile,':');	/* check for : signalling comment */
 int	islogokay = 1;			/* logfile must exist if isstrict */
 if ( commptr != NULL )			/* have embedded comment */
  { strcpy(comment,commptr+1);		/* comment follows : */
    *commptr = '\000'; }		/* null-terminate actual logfile */
 strcpy(logfile,rasteditfilename(logfile)); /* edit log file name */
 if ( *logfile == '\000' ) islogokay = 0; /* given an invalid file name */
 else if ( isstrict ) {			/*okay, but only write if it exists*/
  if ( (logfp=fopen(logfile,"r")) == (FILE *)NULL ) /*doesn't already exist*/
    islogokay = 0;			/* so don't write log file */
  else fclose(logfp); }			/* close file opened for test read */
 if ( islogokay )			/* okay to write logfile */
  if ( (logfp = fopen(logfile,"a"))	/* open logfile */
  != (FILE *)NULL ) {			/* opened successfully for append */
   int	ilog=0;				/* logvars[] index */
   fprintf(logfp,"%s  ",timestamp(TZDELTA,0)); /* first emit timestamp */
   if (*tag=='\000') fprintf(logfp,"%s",filename); /* emit counter filename */
   else fprintf(logfp,"<%s>",tag);	/* or tag if we have one */
   fprintf(logfp,"=%d",counter);	/* emit counter value */
   if ( status < 1 )			/* read or re-write failed */
    fprintf(logfp,"(%s %d)","error status",status); /* emit error */
   for ( ilog=0; logvars[ilog] != NULL; ilog++ ) /* log till end-of-table */
    if ( ilog == commentvar		/* replace with comment... */
    &&   commptr != NULL )		/* ...if available */  
     fprintf(logfp,"  %.256s",comment); /* log embedded comment */
    else
     { char *logval = getenv(logvars[ilog]); /*getenv(variable) to be logged*/
       fprintf(logfp,"  %.64s",		/* log variable */
	(logval!=NULL?logval:"<unknown>")); } /* emit value or <unknown> */
   fprintf(logfp,"\n");			/* terminating newline */
   fclose(logfp);			/* close logfile */
   } /* --- end-of-if(islogokay&&logfp!=NULL) --- */
 } /* --- end-of-if(strlen(logfile)>1) --- */
/* --------------------------------------------------------------------------
construct counter expression and rasterize it
-------------------------------------------------------------------------- */
/* --- construct expression --- */
/*sprintf(text,"%d",counter);*/		/* start with counter */
strcpy(text,dbltoa(((double)counter),0)); /* comma-separated counter value */
if ( ordindex >= 0 )			/* need to tack on ordinal suffix */
  { strcat(text,"^{\\underline{\\rm~");	/* start with ^ and {\underline{\rm */
    strcat(text,ordinal[ordindex]);	/* then st,nd,rd, or th */
    strcat(text,"}}"); }		/* finish with }} */
/* --- rasterize it --- */
rasterize_counter:
  countersp = rasterize(text,size);	/* rasterize counter subexpression */
/* --- return counter image to caller --- */
/*end_of_job:*/
  return ( countersp );			/* return counter image to caller */
} /* --- end-of-function rastcounter() --- */


/* ==========================================================================
 * Function:	rasteval ( expression, size, basesp, arg1, arg2, arg3 )
 * Purpose:	handle \eval
 * --------------------------------------------------------------------------
 * Arguments:	expression (I/O) char **  to first char of null-terminated
 *				string immediately following \eval,
 *				and returning ptr immediately
 *				following last character processed.
 *		size (I)	int containing 0-7 default font size
 *		basesp (I)	subraster *  to character (or subexpression)
 *				immediately preceding \eval
 *				(unused, but passed for consistency)
 *		arg1 (I)	int unused
 *		arg2 (I)	int unused
 *		arg3 (I)	int unused
 * --------------------------------------------------------------------------
 * Returns:	( subraster * )	subraster ptr to image of eval-uated
 *				expression
 * --------------------------------------------------------------------------
 * Notes:     o
 * ======================================================================= */
/* --- entry point --- */
subraster *rasteval ( char **expression, int size, subraster *basesp,
			int arg1, int arg2, int arg3 )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
char	*texsubexpr(), subexpr[MAXSUBXSZ]; /* arg to be evaluated */
subraster *rasterize(), *evalsp=NULL;	/* rasterize evaluated expression */
int	evalterm(), value=0;		/* evaluate expression */
/* -------------------------------------------------------------------------
Parse for subexpr to be \eval-uated, and bump expression past it
-------------------------------------------------------------------------- */
*expression = texsubexpr(*expression,subexpr,0,"{","}",0,0);
if ( *subexpr == '\000' )		/* couldn't get subexpression */
  goto end_of_job;			/* nothing to do, so quit */
/* -------------------------------------------------------------------------
Evaluate expression, ascii-ize integer result, rasterize it
-------------------------------------------------------------------------- */
/* --- evaluate expression --- */
value = evalterm(mimestore,subexpr);	/* evaluate expression */
/* --- ascii-ize it --- */
sprintf(subexpr,"%d",value);		/* ascii version of value */
/* --- rasterize ascii-ized expression value --- */
evalsp = rasterize(subexpr,size);	/* rasterize evaluated expression */
/* --- return evaluated expression raster to caller --- */
end_of_job:
  return ( evalsp );			/* return evaluated expr to caller */
} /* --- end-of-function rasteval() --- */


/* ==========================================================================
 * Function:	rastmathtex ( expression, size, basesp, arg1, arg2, arg3 )
 * Purpose:	handle \mathtex...
 *		  ...obtain pixel image corresponding to expression
 *		by "calling" mathtex, using wget,
 *		for pbm-style bitmap of expression,
 *		and then pixelizing it, and wrapping it in a subraster.
 * --------------------------------------------------------------------------
 * Arguments:	expression (I/O) char **  to first char of null-terminated
 *				string immediately following \mathtex,
 *				and returning ptr immediately
 *				following last character processed.
 *		size (I)	int containing 0-11 default font size
 *				(unused, but passed for consistency)
 *		basesp (I)	subraster *  to character (or subexpression)
 *				immediately preceding \mathtex
 *				(unused, but passed for consistency)
 *		arg1 (I)	int unused
 *		arg2 (I)	int unused
 *		arg3 (I)	int unused
 * --------------------------------------------------------------------------
 * Returns:	( subraster * )	subraster ptr to image of pixelized
 *				pbm bitmap of expression returned by mathtex
 * --------------------------------------------------------------------------
 * Notes:     o	Adapted from function
 *		BYTE *plainmimetext(char *expression, int *width, int *height)
 *		in gifsave89.c
 * ======================================================================= */
/* ---
 * entry point
 * -------------- */
subraster *rastmathtex ( char **expression, int size, subraster *basesp,
			int arg1, int arg2, int arg3 )
{
/* ---
 * allocations and declarations
 * ------------------------------- */
/* --- for mimetex --- */
char	*texsubexpr(), subexpr[MAXSUBXSZ]; /* arg to be evaluated */
subraster *read_pbm(), *mathtexsp=NULL;	/* rasterize mathtex'ed expression */
/* --- for wget() pipe to mathtex --- */
char	command[2048],			/* complete popen(command,"r") */
	execwget[1024];			/* wget either mimetex or mathtex */
FILE	*wget  = NULL;			/* wget's stdout */
char	*localhost="localhost://", *plocalhost=NULL; /*check if running local*/
char	*mathtexpwd = MATHTEXPWD;	/* mathTeX \password{} */
/* ---
 * initialization
 * ----------------- */
/* --- Parse for subexpr for mathtex, and bump expression past it ---*/
*expression = texsubexpr(*expression,subexpr,0,"{","}",0,0);
if ( *subexpr == '\000' )		/* couldn't get subexpression */
  goto end_of_job;			/* nothing to do, so quit */
/* ---
 * set up wget command for webservice
 * ------------------------------------- */
strcpy( execwget,			/* --- mathTeX webservice --- */
	WGET				/*...begins with /path/to/wget */
	" -q -O - "			/*...followed by wget options */
	"\"" MATHTEX "?\\pbm" );	/*..."mathtex.cgi?\pbm\dpi{400} */
if ( !isempty(mathtexpwd) ) {		/* need \password{} ??? */
  strcat(execwget,"\\password{");	/* start with \password{ */
  strcat(execwget,mathtexpwd);		/* then actual mathtex password */
  strcat(execwget,"}"); }		/* and closing } */
strcat(execwget," ");			/* blank space if message needs it */
/* ---
 * open pipe to wget (or run locally)
 * ------------------------------------- */
/* --- first construct popen() command using execwget --- */
strcpy(command,execwget);		/* shell command plus mimetex url */
strcat(command,subexpr);		/* followed by \mathtex{subexpr} */
strcat(command,"\"");			/* and closing " */
/* --- then check for local mathTeX executable --- */
if ( (plocalhost=strstr(MATHTEX,localhost)) /* does url have localhost://? */
!=   NULL ) {				/* if so, replace wget */
  strcpy(command,plocalhost+strlen(localhost)); /*path follows localhost://*/
  strcat(command," \"\\pbm\\stdout ");	/* start with "\pbm\stdout */
  strcat(command,subexpr);		/* followed by \mathtex{subexpr} */
  strcat(command,"\"");			/* and closing " */
  } /* --- end-of-if(plocalhost!=NULL) --- */
/* --- issue wget command and capture its stdout --- */
if ( (wget = popen(command,"r"))	/* issue command and capture stdout*/
== NULL ) goto end_of_job;		/* or quit if failed */
/* ---
 * create subraster for pixelized image
 * --------------------------------------- */
mathtexsp = read_pbm(wget,0.0);
/* ---
 * end-of-job
 * ------------- */
end_of_job:
  if ( wget != NULL ) pclose(wget);	/* close pipe (if open) */
  return ( mathtexsp );			/* back with mathtex subraster */
} /* --- end-of-function rastmathtex() --- */


/* ==========================================================================
 * Function:	rasttoday ( expression, size, basesp, arg1, arg2, arg3 )
 * Purpose:	handle \today
 * --------------------------------------------------------------------------
 * Arguments:	expression (I/O) char **  to first char of null-terminated
 *				string immediately following \today,
 *				and returning ptr immediately
 *				following last character processed.
 *		size (I)	int containing 0-7 default font size
 *		basesp (I)	subraster *  to character (or subexpression)
 *				immediately preceding \today
 *				(unused, but passed for consistency)
 *		arg1 (I)	int unused
 *		arg2 (I)	int unused
 *		arg3 (I)	int unused
 * --------------------------------------------------------------------------
 * Returns:	( subraster * )	subraster ptr to date stamp
 * --------------------------------------------------------------------------
 * Notes:     o
 * ======================================================================= */
/* --- entry point --- */
subraster *rasttoday ( char **expression, int size, subraster *basesp,
			int arg1, int arg2, int arg3 )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
char	*texsubexpr(), optarg[2050];	/* optional [+/-tzdelta,ifmt] args */
char	*timestamp(), *today=optarg;	/* timestamp to be rasterized */
subraster *rasterize(), *todaysp=NULL;	/* rasterize timestamp */
int	ifmt=1, tzdelta=0;		/* default timestamp() args */
/* -------------------------------------------------------------------------
Get optional args \today[+/-tzdelta,ifmt]
-------------------------------------------------------------------------- */
/* --- check for optional \today[+/-tzdelta,ifmt] --- */
if ( *(*expression) == '[' )		/* check for []-enclosed value */
  { *expression = texsubexpr(*expression,optarg,2047,"[","]",0,0);
    if ( *optarg != '\000' )		/* got optional arg */
     { char *comma = strchr(optarg,','); /* comma between +/-tzdelta,ifmt */
       int iarg, nargs=(comma==NULL?1:2); /* #optional args between []'s */
       if ( comma != NULL ) *comma = '\000'; /* null-terminate first arg */
       for ( iarg=1; iarg<=nargs; iarg++ ) /* process one or both args */
	{ char *arg = (iarg==1?optarg:comma+1); /* choose 1st or 2nd arg */
	  if ( isthischar(*arg,"+-") )	/* leading +/- signals tzdelta */
	    tzdelta = atoi(arg);	/* so interpret arg as tzdelta */
	  else ifmt = atoi(arg); }	/* else interpret args as ifmt */
     } /* --- end-of-if(*optarg!='\0') --- */
  } /* --- end-of-if(**expression=='[') --- */
/* -------------------------------------------------------------------------
Get timestamp and rasterize it
-------------------------------------------------------------------------- */
strcpy(today,"\\text{");		/* rasterize timestamp as text */
strcat(today,timestamp(tzdelta,ifmt));	/* get timestamp */
strcat(today,"}");			/* terminate \text{} braces */
todaysp = rasterize(today,size);	/* rasterize timestamp */
/* --- return timestamp raster to caller --- */
/*end_of_job:*/
  return ( todaysp );			/* return timestamp to caller */
} /* --- end-of-function rasttoday() --- */


/* ==========================================================================
 * Function:	rastcalendar ( expression, size, basesp, arg1, arg2, arg3 )
 * Purpose:	handle \calendar
 * --------------------------------------------------------------------------
 * Arguments:	expression (I/O) char **  to first char of null-terminated
 *				string immediately following \calendar
 *				and returning ptr immediately
 *				following last character processed.
 *		size (I)	int containing 0-7 default font size
 *		basesp (I)	subraster *  to character (or subexpression)
 *				immediately preceding \calendar
 *				(unused, but passed for consistency)
 *		arg1 (I)	int unused
 *		arg2 (I)	int unused
 *		arg3 (I)	int unused
 * --------------------------------------------------------------------------
 * Returns:	( subraster * )	subraster ptr to rendered one-month calendar
 * --------------------------------------------------------------------------
 * Notes:     o
 * ======================================================================= */
/* --- entry point --- */
subraster *rastcalendar ( char **expression, int size, subraster *basesp,
			int arg1, int arg2, int arg3 )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
char	*texsubexpr(), optarg[2050];	/* optional [year,month] args */
char	*calendar(), *calstr=NULL;	/* calendar to be rasterized */
subraster *rasterize(), *calendarsp=NULL; /* rasterize calendar string */
int	year=0,month=0,day=0, argval=0;	/* default calendar() args */
/* -------------------------------------------------------------------------
Get optional args \today[+/-tzdelta,ifmt]
-------------------------------------------------------------------------- */
/* --- check for optional \calendar[year,month] --- */
if ( *(*expression) == '[' )		/* check for []-enclosed value */
  { *expression = texsubexpr(*expression,optarg,2047,"[","]",0,0);
    if ( *optarg != '\000' )		/* got optional arg */
     { char *comma = strchr(optarg,','), /* comma between year,month */
       *comma2 = NULL;			/* second comma before day */
       int iarg, nargs=(comma==NULL?1:2); /* #optional args between []'s */
       if ( comma != NULL ) { *comma = '\000'; /*null-terminate first arg*/
	if ( (comma2=strchr(comma+1,',')) != NULL ) /* have third arg */
	 { *comma2 = '\000'; nargs++; } } /* null-term 2nd arg, bump count */
       for ( iarg=1; iarg<=nargs; iarg++ ) /* process one or both args */
	{ char *arg= (iarg==1?optarg:(iarg==2?comma+1:comma2+1)); /*get arg*/
	  argval = atoi(arg);		/* interpret arg as integer */
	  if ( iarg < 3 )		/* first two args are month,year */
	   {if ( argval>1972 && argval<2100 ) year = argval; /* year value */
	    else if ( argval>=1 && argval<=12 ) month = argval;} /*or month*/
	  else				/* only 3rd arg can be day */
	   if ( argval>=1 && argval<=31 ) day = argval; } /* day value */
     } /* --- end-of-if(*optarg!='\0') --- */
  } /* --- end-of-if(**expression=='[') --- */
/* -------------------------------------------------------------------------
Get calendar string and rasterize it
-------------------------------------------------------------------------- */
if ( msgfp!= NULL && msglevel>=9 )
  fprintf(msgfp,"rastcalendar> year=%d, month=%d, day=%d\n",
  year,month,day);
calstr = calendar(year,month,day);		/* get calendar string */
calendarsp = rasterize(calstr,size);	/* rasterize calendar string */
/* --- return calendar raster to caller --- */
/*end_of_job:*/
  return ( calendarsp );		/* return calendar to caller */
} /* --- end-of-function rastcalendar() --- */


/* ==========================================================================
 * Function:	rastenviron ( expression, size, basesp, arg1, arg2, arg3 )
 * Purpose:	handle \environment
 * --------------------------------------------------------------------------
 * Arguments:	expression (I/O) char **  to first char of null-terminated
 *				string immediately following \environment
 *				and returning ptr immediately
 *				following last character processed (in this
 *				case, \environment takes no arguments, so
 *				expression is returned unchanged).
 *		size (I)	int containing 0-7 default font size
 *		basesp (I)	subraster *  to character (or subexpression)
 *				immediately preceding \environment
 *				(unused, but passed for consistency)
 *		arg1 (I)	int unused
 *		arg2 (I)	int unused
 *		arg3 (I)	int unused
 * --------------------------------------------------------------------------
 * Returns:	( subraster * )	subraster ptr to rendered environment image
 * --------------------------------------------------------------------------
 * Notes:     o
 * ======================================================================= */
/* --- entry point --- */
subraster *rastenviron ( char **expression, int size, subraster *basesp,
			int arg1, int arg2, int arg3 )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
char	*texsubexpr(), optarg[255];	/* optional [...] args (for future)*/
char	environstr[8192] = "\000",	/* string for all environment vars */
	environvar[1024] = "\000";	/* one environment variable */
char	*strwrap(),			/* wrap long lines */
	*strdetex(),			/* removes/replaces any math chars */
	*environptr = NULL;		/* ptr to preprocessed environvar */
char	*mimeprep();			/* preprocess environvar string */
int	unescape_url();			/* convert all %xx's to chars */
int	isenviron = (seclevel<=environseclevel?1:0); /*is \environ permitted*/
int	maxvarlen = 512,		/* max chars in environment var */
	maxenvlen = 6400,		/* max chars in entire string */
	wraplen = 48;			/* strwrap() wrap lines at 48 chars*/
int	ienv = 0;			/* environ[] index */
subraster *rasterize(), *environsp=NULL; /* rasterize environment string */
/* -------------------------------------------------------------------------
Get args 
-------------------------------------------------------------------------- */
/* --- check for optional \environment args --- */
if ( 1 )				/* there aren't any args (yet) */
 if ( *(*expression) == '[' ) {		/* check for []-enclosed value */
   *expression = texsubexpr(*expression,optarg,250,"[","]",0,0);
   if ( *optarg != '\000' ) { ;		/* got optional arg, so process it */
     wraplen = atoi(optarg);		/* interpret \environment[wraplen] */
     if ( wraplen < 1 ) wraplen = 8;	/* set minimum */
     } /* --- end-of-if(*optarg!='\0') --- */
   } /* --- end-of-if(**expression=='[') --- */
/* --------------------------------------------------------------------------
emit error message for unauthorized users trying to use \environ
-------------------------------------------------------------------------- */
if ( !isenviron ) {			/* environseclevel > seclevel */
  sprintf(environstr,
  "\\ \\text{[\\backslash environment\\ not permitted]}\\ ");
  goto rasterize_environ;		/* rasterize error message */
  } /* --- end-of-if(!isenviron) --- */
/* -------------------------------------------------------------------------
Accumulate environment variables and rasterize string containing them
-------------------------------------------------------------------------- */
*environstr = '\000';			/* reset environment string */
strcat(environstr,"\\nocaching\\fbox{\\normalsize\\text{"); /*init string*/
for ( ienv=0; ; ienv++ ) {		/* loop over environ[] strings */
  if ( environ[ienv] == (char *)NULL ) break; /* null terminates list */
  if ( *(environ[ienv]) == '\000' ) break; /* double-check empty string */
  strninit(environvar,environ[ienv],maxvarlen); /* max length displayed */
  if ( strlen(environ[ienv]) > maxvarlen ) /* we truncated the variable */
    strcat(environvar,"...");		/* so add an ellipsis */
  unescape_url(environvar,0);		/* convert all %xx's to chars */
  environptr = strdetex(environvar,1);	/* remove/replace any math chars */
  strninit(environvar,environptr,maxvarlen); /*de-tex'ed/nomath environvar*/
  environptr = strwrap(environvar,wraplen,-6); /* wrap long lines */
  strninit(environvar,environptr,maxvarlen); /* line-wrapped environvar */
  mimeprep(environvar);			/* preprocess environvar string */
  if ( strlen(environstr) + strlen(environvar) > maxenvlen ) break;
  sprintf(environstr+strlen(environstr), /* display environment string */
  " %2d. %s\\\\\n", ienv+1,environvar);
  if ( msgfp!= NULL && msglevel>=9 )
    fprintf(msgfp,"rastenviron> %2d. %.256s\n",
    ienv+1,/*environ[ienv]*/environvar);
  if ( strlen(environstr) >= 7200 ) break; /* don't overflow buffer */
  } /* --- end-of-for(ienv) --- */
strcat(environstr,"}}");		/* end {\text{...}} mode */
rasterize_environ:
  environsp = rasterize(environstr,size); /* rasterize environment string */
/* --- return environment raster to caller --- */
/*end_of_job:*/
  return ( environsp );			/* return environment to caller */
} /* --- end-of-function rastenviron() --- */


/* ==========================================================================
 * Function:	rastmessage ( expression, size, basesp, arg1, arg2, arg3 )
 * Purpose:	handle \message
 * --------------------------------------------------------------------------
 * Arguments:	expression (I/O) char **  to first char of null-terminated
 *				string immediately following \message
 *				and returning ptr immediately
 *				following last character processed.
 *		size (I)	int containing 0-7 default font size
 *		basesp (I)	subraster *  to character (or subexpression)
 *				immediately preceding \mesasge
 *				(unused, but passed for consistency)
 *		arg1 (I)	int unused
 *		arg2 (I)	int unused
 *		arg3 (I)	int unused
 * --------------------------------------------------------------------------
 * Returns:	( subraster * )	subraster ptr to rendered message image
 * --------------------------------------------------------------------------
 * Notes:     o
 * ======================================================================= */
/* --- entry point --- */
subraster *rastmessage ( char **expression, int size, subraster *basesp,
			int arg1, int arg2, int arg3 )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
char	*texsubexpr(), amsg[256]="\000"; /* message number text */
int	imsg = 0;			/* default message number */
char	msg[4096];
subraster *rasterize(), *messagesp=NULL; /* rasterize requested message */
int	strreplace();			/*replace SERVER_NAME in refmsgnum*/
int	reflevels = REFLEVELS;		/* #topmost levels to match */
char	*urlprune();			/*prune referer_match in refmsgnum*/
char	*strdetex();			/* remove math chars from messages */
char	*http_host    = getenv("HTTP_HOST"), /* http host for mimeTeX */
	*server_name  = getenv("SERVER_NAME"), /* server hosting mimeTeX */
	*referer_match = (!isempty(http_host)?http_host: /*match http_host*/
	  (!isempty(server_name)?server_name:(NULL))); /* or server_name */
/* -------------------------------------------------------------------------
obtain message {amsg} argument
-------------------------------------------------------------------------- */
/* --- parse for {amsg} arg, and bump expression past it --- */
*expression = texsubexpr(*expression,amsg,255,"{","}",0,0);
/* --- interpret argument --- */
if ( *amsg != '\000' ) {		/* got amsg arg */
  imsg = atoi(amsg);			/* interpret as an int */
  if ( imsg < 0				/* if too small */
  ||   imsg > maxmsgnum )		/* or too big */
    imsg = 0; }				/* default to first message */
/* --- retrieve requested message --- */
strninit(msg,msgtable[imsg],4095);	/* local copy of message */
/* --- process as necessary --- */
if ( imsg == refmsgnum) {		/* urlncmp() failed to validate */
  if ( reflevels > 0 )			/* have #levels to validate */
   strreplace(msg,"SERVER_NAME",	/* replace SERVER_NAME */
    strdetex(urlprune(referer_match,reflevels),1),0); /*with referer_match*/
  } /* --- end-of-switch(imsg) --- */
/* --- rasterize requested message --- */
messagesp = rasterize(msg,size);	/* rasterize message string */
/* --- return message raster to caller --- */
/*end_of_job:*/
  return ( messagesp );			/* return message to caller */
} /* --- end-of-function rastmessage() --- */


/* ==========================================================================
 * Function:	rastnoop ( expression, size, basesp, nargs, arg2, arg3 )
 * Purpose:	no op -- flush \escape without error
 * --------------------------------------------------------------------------
 * Arguments:	expression (I/O) char **  to first char of null-terminated
 *				string immediately following \escape to be
 *				flushed, and returning ptr immediately
 *				following last character processed.
 *		size (I)	int containing 0-5 default font size
 *		basesp (I)	subraster *  to character (or subexpression)
 *				immediately preceding \escape
 *				(unused, but passed for consistency)
 *		nargs (I)	int containing number of {}-args after
 *				\escape to be flushed along with it
 *		arg2 (I)	int unused
 *		arg3 (I)	int unused
 * --------------------------------------------------------------------------
 * Returns:	( subraster * )	NULL subraster ptr
 * --------------------------------------------------------------------------
 * Notes:     o
 * ======================================================================= */
/* --- entry point --- */
subraster *rastnoop ( char **expression, int size, subraster *basesp,
			int nargs, int arg2, int arg3 )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
char	*texsubexpr(), subexpr[MAXSUBXSZ+1]; /*dummy args eaten by \escape*/
subraster *rasterize(), *noopsp=NULL;	/* rasterize subexpr */
/* --- flush accompanying args if necessary --- */
if ( nargs != NOVALUE			/* not unspecified */
&&   nargs > 0 )			/* and args to be flushed */
  while ( --nargs >= 0 )		/* count down */
    *expression = texsubexpr(*expression,subexpr,0,"{","}",0,0); /*flush arg*/
/* --- return null ptr to caller --- */
/*end_of_job:*/
  return ( noopsp );			/* return NULL ptr to caller */
} /* --- end-of-function rastnoop() --- */


/* ==========================================================================
 * Function:	rastopenfile ( filename, mode )
 * Purpose:	Opens filename[.tex] in mode, returning FILE *
 * --------------------------------------------------------------------------
 * Arguments:	filename (I/O)	char * to null-terminated string containing
 *				name of file to open (preceded by path
 *				relative to mimetex executable)
 *				If fopen() fails, .tex appeneded,
 *				and returned if that fopen() succeeds
 *		mode (I)	char * to null-terminated string containing
 *				fopen() mode
 * --------------------------------------------------------------------------
 * Returns:	( FILE * )	pointer to opened file, or NULL if error
 * --------------------------------------------------------------------------
 * Notes:     o
 * ======================================================================= */
/* --- entry point --- */
FILE	*rastopenfile ( char *filename, char *mode )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
FILE	*fp = (FILE *)NULL /*,*fopen()*/; /*file pointer to opened filename*/
char	texfile[2050] = "\000",		/* local, edited copy of filename */
	*rasteditfilename(),		/* prepend pathprefix if necessary */
	amode[512] = "r";		/* test open mode if arg mode=NULL */
int	ismode = 0;			/* true of mode!=NULL */
/* --------------------------------------------------------------------------
Check mode and open file
-------------------------------------------------------------------------- */
/* --- edit filename --- */
strncpy(texfile,rasteditfilename(filename),2047); /*edited copy of filename*/
texfile[2047] = '\000';			/* make sure it's null terminated */
/* --- check mode --- */
if ( mode != (char *)NULL )		/* caller passed mode arg */
 if ( *mode != '\000' )			/* and it's not an empty string */
  { ismode = 1;				/* so flip mode flag true */
    strncpy(amode,mode,254);		/* and replace "r" with caller's */
    amode[254] = '\000';		/* make sure it's null terminated */
    compress(amode,' '); }		/* remove embedded blanks */
/* --- open filename or filename.tex --- */
if ( strlen(texfile) > 1 )		/* make sure we got actual filename*/
  if ( (fp = fopen(texfile,amode))	/* try opening given filename */
  ==   NULL )				/* failed to open given filename */
  { strcpy(filename,texfile);		/* signal possible filename error */
    strcat(texfile,".tex");		/* but first try adding .tex */
    if ( (fp = fopen(texfile,amode))	/* now try opening filename.tex */
    !=   NULL )				/* filename.tex succeeded */
      strcpy(filename,texfile); }	/* replace caller's filename */
/* --- close file if only opened to check name --- */
if ( !ismode && fp!=NULL )		/* no mode, so just checking */
  fclose(fp);				/* close file, fp signals success */
/* --- return fp or NULL to caller --- */
/*end_of_job:*/
  if ( msglevel>=9 && msgfp!=NULL )	/* debuging */
    { fprintf(msgfp,"rastopenfile> returning fopen(%s,%s) = %s\n",
      filename,amode,(fp==NULL?"NULL":"Okay")); fflush(msgfp); }
  return ( fp );			/* return fp or NULL to caller */
} /* --- end-of-function rastopenfile() --- */


/* ==========================================================================
 * Function:	rasteditfilename ( filename )
 * Purpose:	edits filename to remove security problems,
 *		e.g., removes all ../'s and ..\'s.
 * --------------------------------------------------------------------------
 * Arguments:	filename (I)	char * to null-terminated string containing
 *				name of file to be edited
 * --------------------------------------------------------------------------
 * Returns:	( char * )	pointer to edited filename,
 *				or empty string "\000" if any problem
 * --------------------------------------------------------------------------
 * Notes:     o
 * ======================================================================= */
/* --- entry point --- */
char	*rasteditfilename ( char *filename )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
static	char editname[2050];		/*edited filename returned to caller*/
char	*strchange();			/* prepend pathprefix if necessary */
int	strreplace(),			/* remove ../'s and ..\'s */
	isprefix = (*pathprefix=='\000'?0:1); /* true if paths have prefix */
/* --------------------------------------------------------------------------
edit filename
-------------------------------------------------------------------------- */
/* --- first check filename arg --- */
*editname = '\000';			/* init edited name as empty string*/
if ( filename == (char *)NULL ) goto end_of_job; /* no filename arg */
if ( *filename == '\000' ) goto end_of_job; /* filename is an empty string */
/* --- init edited filename --- */
strcpy(editname,filename);		/* init edited name as input name */
compress(editname,' ');			/* remove embedded blanks */
/* --- remove leading or embedded ....'s --- */
while ( strreplace(editname,"....",NULL,0) > 0 ) ;  /* squeeze out ....'s */
/* --- remove leading / and \ and dots (and blanks) --- */
if ( *editname != '\000' )		/* still have chars in filename */
 while ( isthischar(*editname," ./\\") ) /* absolute paths invalid */
   {strsqueeze(editname,1);}		/* so flush leading / or \ (or ' ')*/
if ( *editname == '\000' ) goto end_of_job; /* no chars left in filename */
/* --- remove leading or embedded ../'s and ..\'s --- */
while ( strreplace(editname,"../",NULL,0) > 0 ) ;  /* squeeze out ../'s */
while ( strreplace(editname,"..\\",NULL,0) > 0 ) ; /* and ..\'s */
while ( strreplace(editname,"../",NULL,0) > 0 ) ;  /* and ../'s again */
/* --- prepend path prefix (if compiled with -DPATHPREFIX) --- */
if ( isprefix && *editname!='\000' )	/* filename is preceded by prefix */
  strchange(0,editname,pathprefix);	/* so prepend prefix */
end_of_job:
  return ( editname );			/* back with edited filename */
} /* --- end-of-function rasteditfilename() --- */


/* ==========================================================================
 * Function:	rastreadfile ( filename, islock, tag, value )
 * Purpose:	Read filename, returning value as string
 *		between <tag>...</tag> or entire file if tag=NULL passed.
 * --------------------------------------------------------------------------
 * Arguments:	filename (I)	char * to null-terminated string containing
 *				name of file to read (preceded by path
 *				relative to mimetex executable)
 *		islock (I)	int containing 1 to lock file while reading
 *				(hopefully done by opening in "r+" mode)
 *		tag (I)		char * to null-terminated string containing
 *				html-like tagname.  File contents between
 *				<tag> and </tag> will be returned, or
 *				entire file if tag=NULL passed.
 *		value (O)	char * returning value between <tag>...</tag>
 *				or entire file if tag=NULL.
 * --------------------------------------------------------------------------
 * Returns:	( int )		1=okay, 0=some error
 * --------------------------------------------------------------------------
 * Notes:     o
 * ======================================================================= */
/* --- entry point --- */
int	rastreadfile ( char *filename, int islock, char *tag, char *value )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
FILE	*fp = (FILE *)NULL, *rastopenfile(); /* pointer to opened filename */
char	texfile[1024] = "\000",		/* local copy of input filename */
	text[MAXLINESZ+1];		/* line from input file */
char	*tagp, tag1[1024], tag2[1024];	/* left <tag> and right <tag/> */
int	vallen=0, maxvallen=MAXFILESZ;	/* #chars in value, max allowed */
int	status = (-1);			/* status returned, 1=okay */
int	tagnum = 0;			/* tag we're looking for */
/*int	islock = 1;*/			/* true to lock file */
/* --------------------------------------------------------------------------
Open file
-------------------------------------------------------------------------- */
/* --- first check output arg --- */
if ( value == (char *)NULL ) goto end_of_job; /* no output buffer supplied */
*value = '\000';			/* init buffer with empty string */
/* --- open filename or filename.tex --- */
if ( filename != (char *)NULL )		/* make sure we got filename arg */
  { strncpy(texfile,filename,1023);	/* local copy of filename */
    texfile[1023] = '\000';		/* make sure it's null terminated */
    fp = rastopenfile(texfile,(islock?"r+":"r")); } /* try opening it */
/* --- check that file opened --- */
if ( fp == (FILE *)NULL )		/* failed to open file */
  { sprintf(value,"{\\normalsize\\rm[file %s?]}",texfile);
    goto end_of_job; }			/* return error message to caller */
status = 0;				/* file opened successfully */
if ( islock ) rewind(fp);		/* start at beginning of file */
/* --------------------------------------------------------------------------
construct <tag>'s
-------------------------------------------------------------------------- */
if ( tag != (char *)NULL )		/* caller passed tag arg */
 if ( *tag != '\000' )			/* and it's not an empty string */
  { strcpy(tag1,"<"); strcpy(tag2,"</"); /* begin with < and </ */
    strcat(tag1,tag); strcat(tag2,tag);	/* followed by caller's tag */
    strcat(tag1,">"); strcat(tag2,">");	/* ending both tags with > */
    compress(tag1,' '); compress(tag2,' '); /* remove embedded blanks */
    tagnum = 1; }			/* signal that we have tag */
/* --------------------------------------------------------------------------
Read file, concatnate lines
-------------------------------------------------------------------------- */
while ( fgets(text,MAXLINESZ-1,fp) != (char *)NULL ) { /*read input till eof*/
  switch ( tagnum ) {			/* look for left- or right-tag */
    case 0: status = 1; break;		/* no tag to look for */
    case 1:				/* looking for opening left <tag> */
      if ( (tagp=strstr(text,tag1)) == NULL ) break; /*haven't found it yet*/
      tagp += strlen(tag1);		/* first char past tag */
      strsqueezep(text,tagp);		/*shift out preceding text and tag*/
      tagnum = 2;			/*now looking for closing right tag*/
    case 2:				/* looking for closing right </tag> */
      if ( (tagp=strstr(text,tag2)) == NULL ) break; /*haven't found it yet*/
      *tagp = '\000';			/* terminate line at tag */
      tagnum = 3;			/* done after this line */
      status = 1;			/* successfully read tag */
      break;
    } /* ---end-of-switch(tagnum) --- */
  if ( tagnum != 1 ) {			/* no tag or left tag already found*/
    int	textlen = strlen(text);		/* #chars in current line */
    if ( vallen+textlen > maxvallen ) break; /* quit before overflow */
    strcat(value,text);			/* concat line to end of value */
    vallen += textlen;			/* bump length */
    if ( tagnum > 2 ) break; }		/* found right tag, so we're done */
  } /* --- end-of-while(fgets()!=NULL) --- */
if ( tagnum<1 || tagnum>2 ) status=1;	/* okay if no tag or we found tag */
fclose ( fp );				/* close input file after reading */
/* --- return value and status to caller --- */
end_of_job:
  return ( status );			/* return status to caller */
} /* --- end-of-function rastreadfile() --- */


/* ==========================================================================
 * Function:	rastwritefile ( filename, tag, value, isstrict )
 * Purpose:	Re/writes filename, replacing string between <tag>...</tag>
 *		with value, or writing entire file as value if tag=NULL.
 * --------------------------------------------------------------------------
 * Arguments:	filename (I)	char * to null-terminated string containing
 *				name of file to write (preceded by path
 *				relative to mimetex executable)
 *		tag (I)		char * to null-terminated string containing
 *				html-like tagname.  File contents between
 *				<tag> and </tag> will be replaced, or
 *				entire file written if tag=NULL passed.
 *		value (I)	char * containing string replacing value
 *				between <tag>...</tag> or replacing entire
 *				file if tag=NULL.
 *		isstrict (I)	int containing 1 to only rewrite existing
 *				files, or 0 to create new file if necessary.
 * --------------------------------------------------------------------------
 * Returns:	( int )		1=okay, 0=some error
 * --------------------------------------------------------------------------
 * Notes:     o
 * ======================================================================= */
/* --- entry point --- */
int	rastwritefile( char *filename, char *tag, char *value, int isstrict )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
FILE	*fp = (FILE *)NULL, *rastopenfile(); /* pointer to opened filename */
char	texfile[1024] = "\000",		/* local copy of input filename */
	filebuff[MAXFILESZ+1] = "\000",	/* entire contents of file */
	tag1[1024], tag2[1024],		/* left <tag> and right <tag/> */
	*strchange(),			/* put value between <tag>...</tag>*/
	*timestamp();			/* log modification time */
int	istag=0, rastreadfile(),	/* read file if tag!=NULL */
	/*isstrict = (seclevel>5? 1:0),*/ /*true only writes existing files*/
	isnewfile = 0,			/* true if writing new file */
	status = 0;			/* status returned, 1=okay */
int	istimestamp = 0;		/* true to update <timestamp> tag */
/* --------------------------------------------------------------------------
check args
-------------------------------------------------------------------------- */
/* --- check filename and value --- */
if ( filename == (char *)NULL		/* quit if no filename arg supplied*/
||   value == (char *)NULL ) goto end_of_job; /* or no value arg supplied */
if ( strlen(filename) < 2		/* quit if unreasonable filename */
||   *value == '\000' ) goto end_of_job; /* or empty value string supplied */
/* --- establish filename[.tex] --- */
strncpy(texfile,filename,1023);		/* local copy of input filename */
texfile[1023] = '\000';			/* make sure it's null terminated */
if ( rastopenfile(texfile,NULL)		/* unchanged or .tex appended */
==   (FILE *)NULL )			/* can't open, so write new file */
  { if ( isstrict ) goto end_of_job;	/* fail if new files not permitted */
    isnewfile = 1; }			/* signal we're writing new file */
/* --- check whether tag supplied by caller --- */
if ( tag != (char *)NULL )		/* caller passed tag argument */
 if ( *tag != '\000' )			/* and it's not an empty string */
  { istag = 1;				/* so flip tag flag true */
    strcpy(tag1,"<"); strcpy(tag2,"</");  /* begin tags with < and </ */
    strcat(tag1,tag); strcat(tag2,tag);   /* followed by caller's tag */
    strcat(tag1,">"); strcat(tag2,">");	/* ending both tags with > */
    compress(tag1,' '); compress(tag2,' '); } /* remove embedded blanks */
/* --------------------------------------------------------------------------
read existing file if just rewriting a single tag
-------------------------------------------------------------------------- */
/* --- read original file if only replacing a tag within it --- */
*filebuff = '\000';			/* init as empty file */
if ( !isnewfile )			/* if file already exists */
 if ( istag )				/* and just rewriting one tag */
  if ( rastreadfile(texfile,1,NULL,filebuff) /* read entire existing file */
  <=   0 ) goto end_of_job;		/* signal error if failed to read */
/* --------------------------------------------------------------------------
construct new file data if needed (entire file replaced by value if no tag)
-------------------------------------------------------------------------- */
if ( istag )				/* only replacing tag in file */
 {
 /* --- find <tag> and </tag> in file --- */
 int	tlen1=strlen(tag1),  tlen2=strlen(tag2), flen;  /*tag,buff lengths*/
 char	*tagp1 = (isnewfile? NULL:strstr(filebuff,tag1)), /* <tag> in file*/
	*tagp2 = (isnewfile? NULL:strstr(filebuff,tag2)); /*</tag> in file*/
 /* --- if adding new <tag> just concatanate at end of file --- */
 if ( tagp1 == (char *)NULL )		/* add new tag to file */
  {
  /* --- preprocess filebuff --- */
  if ( tagp2 != (char *)NULL )		/* apparently have ...</tag> */
    {strsqueezep(filebuff,tagp2+tlen2);} /* remove ...</tag> */
  if ( (flen = strlen(filebuff))	/* #chars currently in buffer */
  > 0 )					/* we have non-empty buffer */
   if (!isthischar(*(filebuff+flen-1),"\n\r")) /*no newline at end of file*/
     if(0)strcat(filebuff,"\n");	/* so add one before new tag */
  /* --- add new tag --- */
  strcat(filebuff,tag1);		/* add opening <tag> */
  strcat(filebuff,value);		/* then value */
  strcat(filebuff,tag2);		/* finally closing </tag> */
  strcat(filebuff,"\n");		/* newline at end of file */
  } /* --- end-of-if(tagp1==NULL) --- */
 else					/* found existing opening <tag> */
  {
  if ( tagp2 == NULL )			/* apparently have <tag>... */
    { *(tagp1+tlen1) = '\000';		/* so get rid of trailing ... */
      strcat(filebuff,value);		/* then concatanate value */
      strcat(filebuff,tag2); }		/* and finally closing </tag> */
  else					/* else have <tag>...<tag/> */
   if ( (flen=((int)(tagp2-tagp1))-tlen1) /* len of .'s in <tag>...</tag> */
   >=   0 )				/* usually <tag> precedes </tag> */
    strchange(flen,tagp1+tlen1,value);	/* change ...'s to value */
   else					/* weirdly, </tag> precedes <tag> */
    { char fbuff[4096];			/* field buff for <tag>value</tag> */
      if ( (flen = ((int)(tagp1-tagp2))+tlen1) /* strlen(</tag>...<tag>) */
      <=   0 ) goto end_of_job;		/* must be internal error */
      strcpy(fbuff,tag1);		/* set opening <tag> */
      strcat(fbuff,value);		/* then value */
      strcat(fbuff,tag2);		/* finally closing </tag> */
      strchange(flen,tagp2,fbuff); }	/* replace original </tag>...<tag> */
  } /* --- end-of-if/else(tagp1==NULL) --- */
 } /* --- end-of-if(istag) --- */
/* --------------------------------------------------------------------------
rewrite file and return to caller
-------------------------------------------------------------------------- */
/* --- first open file for write --- */
if ( (fp=rastopenfile(texfile,"w"))	/* open for write */
==   (FILE *)NULL ) goto end_of_job;	/* signal error if can't open */
/* --- rewrite and close file --- */
if ( fputs((istag?filebuff:value),fp)	/* write filebuff or value */
!=  EOF ) status = 1;			/* signal success if succeeded */
fclose ( fp );				/* close output file after writing */
/* --- modify timestamp --- */
if ( status > 0 )			/*forget timestamp if write failed*/
 if ( istimestamp )			/* if we're updating timestamp */
  if ( istag )				/* only log time in tagged file */
   if ( strstr(tag,"timestamp") == (char *)NULL ) /* but avoid recursion */
    { char fbuff[2048];			/* field buff <timestamp> value */
      strcpy(fbuff,tag);		/* tag modified */
      strcat(fbuff," modified at ");	/* spacer */
      strcat(fbuff,timestamp(TZDELTA,0)); /* start with timestamp */
      status = rastwritefile(filename,"timestamp",fbuff,1); }
/* --- return status to caller --- */
end_of_job:
  return ( status );			/* return status to caller */
} /* --- end-of-function rastwritefile() --- */


/* ==========================================================================
 * Function:	calendar ( year, month, day )
 * Purpose:	returns null-terminated character string containing
 *		\begin{array}...\end{array} for the one-month calendar
 *		specified by year=1973...2099 and month=1...12.
 *		If either arg out-of-range, today's value is used.
 * --------------------------------------------------------------------------
 * Arguments:	year (I)	int containing 1973...2099 or 0 for current
 *				year
 *		month (I)	int containing 1...12 or 0 for current month
 *		day (I)		int containing day to emphasize or 0
 * --------------------------------------------------------------------------
 * Returns:	( char * )	char ptr to null-terminated buffer
 *				containing \begin{array}...\end{array}
 *				string that will render calendar for
 *				requested month, or NULL for any error.
 * --------------------------------------------------------------------------
 * Notes:     o
 * ======================================================================= */
/* --- entry point --- */
char	*calendar( int year, int month, int day )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
static char calbuff[4096];		/* calendar returned to caller */
time_t	time_val = (time_t)(0);		/* binary value returned by time() */
struct tm *tmstruct=(struct tm *)NULL, *localtime(); /* interpret time_val */
int	yy=0, mm=0, dd=0;		/* today (emphasize today's dd) */
int	idd=1, iday=0, daynumber();	/* day-of-week for idd=1...31 */
char	aval[64];			/* ascii day or 4-digit year */
/* --- calendar data --- */
static	char *monthnames[] = { "?", "January", "February", "March", "April",
	 "May", "June", "July", "August", "September", "October",
	"November", "December", "?" } ;
static	int modays[] =
	{ 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 0 };
/* -------------------------------------------------------------------------
initialization
-------------------------------------------------------------------------- */
/* --- get current date/time --- */
time((time_t *)(&time_val));		/* get date and time */
tmstruct = localtime((time_t *)(&time_val)); /* interpret time_val */
yy  =  1900 + (int)(tmstruct->tm_year);	/* current four-digit year */
mm  =  1 + (int)(tmstruct->tm_mon);	/* current month, 1-12 */
dd  =  (int)(tmstruct->tm_mday);	/* current day, 1-31 */
/* --- check args --- */
if ( year<1973 || year>2099 ) year  = yy; /* current year if out-of-bounds */
if ( month<1 || month>12 ) month = mm;	/* current month if out-of-bounds */
if ( month==mm && year==yy && day==0 )	/* current month and default day */
  day = dd;				/* emphasize current day */
modays[2] = (year%4==0?29:28);		/* Feb has 29 days in leap years */
/* --- initialize calendar string --- */
strcpy(calbuff,"{\\begin{gather}");	/* center `month year` above cal */
strcat(calbuff,"\\small\\text{");	/* month set in roman */
strcat(calbuff,monthnames[month]);	/* insert month name */
strcat(calbuff," }");			/* add a space */
sprintf(aval,"%d",year);		/* convert year to ascii */
strcat(calbuff,aval);			/* add year */
strcat(calbuff,"\\\\");			/* end top row */
strcat(calbuff,				/* now begin calendar arrayr */
	"\\begin{array}{|c|c|c|c|c|c|c|CCCCCC} \\hline"
	"\\tiny\\text{Sun} & \\tiny\\text{Mon} & \\tiny\\text{Tue} &"
	"\\tiny\\text{Wed} & \\tiny\\text{Thu} & \\tiny\\text{Fri} &"
	"\\tiny\\text{Sat} \\\\ \\hline " );
/* -------------------------------------------------------------------------
generate calendar
-------------------------------------------------------------------------- */
for ( idd=1; idd<=modays[month]; idd++ ) /* run through days of month */
  {
  /* --- get day-of-week for this day --- */
  iday = 1 + (daynumber(year,month,idd)%7); /* 1=Monday...7=Sunday */
  if ( iday == 7 ) iday = 0;		/* now 0=Sunday...6=Saturday */
  /* --- may need empty cells at beginning of month --- */
  if ( idd == 1 )			/* first day of month */
   if ( iday > 0 )			/* need to skip cells */
    { strcpy(aval,"\\ &\\ &\\ &\\ &\\ &\\ &\\ &\\ &\\ &\\"); /*cells to skip*/
      aval[3*iday] = '\000';		/*skip cells preceding 1st of month*/
      strcat(calbuff,aval); }		/* add skip string to buffer */
  /* --- add idd to current cell --- */
  sprintf(aval,"%d",idd);		/* convert idd to ascii */
  if ( idd == day			/* emphasize today's date */
  /*&&   month==mm && year==yy*/ )	/* only if this month's calendar */
   { strcat(calbuff,"{\\fs{-1}\\left\\langle "); /*emphasize, 1 size smaller*/
     strcat(calbuff,aval);		/* put in idd */
     strcat(calbuff,"\\right\\rangle}"); } /* finish emphasis */
  else					/* not today's date */
    strcat(calbuff,aval);		/* so just put in idd */
  /* --- terminate cell --- */
  if ( idd < modays[month] ) {		/* not yet end-of-month */
   if ( iday < 6 )			/* still have days left in week */
    strcat(calbuff,"&");		/* new cell in same week */
   else					/* reached end-of-week */
    strcat(calbuff,"\\\\ \\hline"); }	/* so start new week */
  } /* --- end-of-for(idd) --- */
strcat(calbuff,"\\\\ \\hline");		/* final underline at end-of-month */
/* --- return calendar to caller --- */
strcat(calbuff,"\\end{array}\\end{gather}}"); /* terminate array */
return ( calbuff );			/* back to caller with calendar */
} /* --- end-of-function calendar() --- */


/* ==========================================================================
 * Function:	timestamp ( tzdelta, ifmt )
 * Purpose:	returns null-terminated character string containing
 *		current date:time stamp as ccyy-mm-dd:hh:mm:ss{am,pm}
 * --------------------------------------------------------------------------
 * Arguments:	tzdelta (I)	integer, positive or negative, containing
 *				containing number of hours to be added or
 *				subtracted from system time (to accommodate
 *				your desired time zone).
 *		ifmt (I)	integer containing 0 for default format
 * --------------------------------------------------------------------------
 * Returns:	( char * )	ptr to null-terminated buffer
 *				containing current date:time stamp
 * --------------------------------------------------------------------------
 * Notes:     o
 * ======================================================================= */
/* --- entry point --- */
char	*timestamp( int tzdelta, int ifmt )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
static	char timebuff[256];		/* date:time buffer back to caller */
/*long	time_val = 0L;*/		/* binary value returned by time() */
time_t	time_val = (time_t)(0);		/* binary value returned by time() */
struct tm *tmstruct=(struct tm *)NULL, *localtime(); /* interpret time_val */
int	year=0, hour=0,ispm=1,		/* adjust year, and set am/pm hour */
	month=0, day=0,			/* adjust day and month for delta  */
	minute=0,second=0;		/* minute and second not adjusted  */
int	tzadjust();			/* time zone adjustment function */
int	daynumber();			/* #days since Jan 1, 1973 */
static	char *daynames[] = { "Monday", "Tuesday", "Wednesday",
	 "Thursday", "Friday", "Saturday", "Sunday" } ;
static	char *monthnames[] = { "?", "January", "February", "March", "April",
	 "May", "June", "July", "August", "September", "October",
	"November", "December", "?" } ;
/* -------------------------------------------------------------------------
get current date:time, adjust values, and and format stamp
-------------------------------------------------------------------------- */
/* --- first init returned timebuff in case of any error --- */
*timebuff = '\000';
/* --- get current date:time --- */
time((time_t *)(&time_val));		/* get date and time */
tmstruct = localtime((time_t *)(&time_val)); /* interpret time_val */
/* --- extract fields --- */
year  = (int)(tmstruct->tm_year);	/* local copy of year,  0=1900 */
month = (int)(tmstruct->tm_mon) + 1;	/* local copy of month, 1-12 */
day   = (int)(tmstruct->tm_mday);	/* local copy of day,   1-31 */
hour  = (int)(tmstruct->tm_hour);	/* local copy of hour,  0-23 */
minute= (int)(tmstruct->tm_min);	/* local copy of minute,0-59 */
second= (int)(tmstruct->tm_sec);	/* local copy of second,0-59 */
/* --- adjust year --- */
year += 1900;				/* set century in year */
/* --- adjust for timezone --- */
tzadjust(tzdelta,&year,&month,&day,&hour);
/* --- check params --- */
if ( hour<0  || hour>23
||   day<1   || day>31
||   month<1 || month>12
||   year<1973 ) goto end_of_job;
/* --- adjust hour for am/pm --- */
switch ( ifmt )
  {
  default:
  case 0:
    if ( hour < 12 )			/* am check */
     { ispm=0;				/* reset pm flag */
       if ( hour == 0 ) hour = 12; }	/* set 00hrs = 12am */
    if ( hour > 12 ) hour -= 12;	/* pm check sets 13hrs to 1pm, etc */
    break;
  } /* --- end-of-switch(ifmt) --- */
/* --- format date:time stamp --- */
switch ( ifmt )
  {
  default:
  case 0:  /* --- 2005-03-05:11:49:59am --- */
    sprintf(timebuff,"%04d-%02d-%02d:%02d:%02d:%02d%s", year,month,day,
    hour,minute,second,((ispm)?"pm":"am"));
    break;
  case 1:  /* --- Saturday, March 5, 2005 --- */
    sprintf(timebuff,"%s, %s %d, %d",
    daynames[daynumber(year,month,day)%7],monthnames[month],day,year);
    break;
  case 2: /* --- Saturday, March 5, 2005, 11:49:59am --- */
    sprintf(timebuff,"%s, %s %d, %d, %d:%02d:%02d%s",
    daynames[daynumber(year,month,day)%7],monthnames[month],day,year,
    hour,minute,second,((ispm)?"pm":"am"));
    break;
  case 3: /* --- 11:49:59am --- */
    sprintf(timebuff,"%d:%02d:%02d%s",
    hour,minute,second,((ispm)?"pm":"am"));
    break;
  case 4: /* --- 1231235959 (mmddhhmmss time as integer) --- */
    sprintf(timebuff,"%d%02d%02d%02d%02d",
    month,day,hour,minute,second);
    break;
  } /* --- end-of-switch(ifmt) --- */
end_of_job:
  return ( timebuff );			/* return stamp to caller */
} /* --- end-of-function timestamp() --- */


/* ==========================================================================
 * Function:	tzadjust ( tzdelta, year, month, day, hour )
 * Purpose:	Adjusts hour, and day,month,year if necessary,
 *		by delta increment to accommodate your time zone.
 * --------------------------------------------------------------------------
 * Arguments:	tzdelta (I)	integer, positive or negative, containing
 *				containing number of hours to be added or
 *				subtracted from given time (to accommodate
 *				your desired time zone).
 *		year (I)	addr of int containing        4-digit year
 *		month (I)	addr of int containing month  1=Jan - 12=Dec.
 *		day (I)		addr of int containing day    1-31 for Jan.
 *		hour (I)	addr of int containing hour   0-23
 * Returns:	( int )		1 for success, or 0 for error
 * --------------------------------------------------------------------------
 * Notes:     o
 * ======================================================================= */
/* --- entry point --- */
int	tzadjust ( int tzdelta, int *year, int *month, int *day, int *hour )
{
/* --------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
int	yy = *year, mm = *month, dd = *day, hh = *hour; /*dereference args*/
/* --- calendar data --- */
static	int modays[] =
	{ 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 0 };
/* --------------------------------------------------------------------------
check args
-------------------------------------------------------------------------- */
if ( mm<1 || mm>12 ) return(-1);	/* bad month */
if ( dd<1 || dd>modays[mm] ) return(-1); /* bad day */
if ( hh<0 || hh>23 ) return(-1);	/* bad hour */
if ( tzdelta>23 || tzdelta<(-23) ) return(-1); /* bad tzdelta */
/* --------------------------------------------------------------------------
make adjustments
-------------------------------------------------------------------------- */
/* --- adjust hour --- */
hh += tzdelta;				/* apply caller's delta */
/* --- adjust for feb 29 --- */
modays[2] = (yy%4==0?29:28);		/* Feb has 29 days in leap years */
/* --- adjust day --- */
if ( hh < 0 )				/* went to preceding day */
  { dd--;  hh += 24; }
if ( hh > 23 )				/* went to next day */
  { dd++;  hh -= 24; }
/* --- adjust month --- */
if ( dd < 1 )				/* went to preceding month */
  { mm--;  dd = modays[mm]; }
if ( dd > modays[mm] )			/* went to next month */
  { mm++;  dd = 1; }
/* --- adjust year --- */
if ( mm < 1 )				/* went to preceding year */
  { yy--;  mm = 12;  dd = modays[mm]; }
if ( mm > 12 )				/* went to next year */
  { yy++;  mm = 1;   dd = 1; }
/* --- back to caller --- */
*year=yy; *month=mm; *day=dd; *hour=hh;	/* reset adjusted args */
return ( 1 );
} /* --- end-of-function tzadjust() --- */


/* ==========================================================================
 * Function:	daynumber ( year, month, day )
 * Purpose:	Returns number of actual calendar days from Jan 1, 1973
 *		to the given date (e.g., bvdaynumber(1974,1,1)=365).
 * --------------------------------------------------------------------------
 * Arguments:	year (I)	int containing year -- may be either 1995 or
 *				95, or may be either 2010 or 110 for those
 *				years.
 *		month (I)	int containing month, 1=Jan thru 12=Dec.
 *		day (I)		int containing day of month, 1-31 for Jan, etc.
 * Returns:	( int )		Number of days from Jan 1, 1973 to given date,
 *				or -1 for error (e.g., year<1973).
 * --------------------------------------------------------------------------
 * Notes:     o
 * ======================================================================= */
/* --- entry point --- */
int	daynumber ( int year, int month, int day )
{
/* --------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
/* --- returned value (note: returned as a default "int") --- */
int	ndays;				/* #days since jan 1, year0 */
/* --- initial conditions --- */
static	int year0 = 73, 		/* jan 1 was a monday, 72 was a leap */
	days4yrs = 1461,		/* #days in 4 yrs = 365*4 + 1 */
	days1yr  = 365;
/* --- table of accumulated days per month (last index not used) --- */
static	int modays[] =
	{ 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 };
/* --- variables for #days since day0 --- */
int	nyears, nfouryrs;		/*#years, #4-yr periods since year0*/
/* --------------------------------------------------------------------------
Check input
-------------------------------------------------------------------------- */
if ( month < 1 || month > 12 )		/*month used as index, so must be ok*/
	return ( -1 );			/* otherwise, forget it */
if ( year >= 1900 ) year -= 1900;	/*use two-digit years (3 after 2000)*/
/* --------------------------------------------------------------------------
Find #days since jan 1, 1973
-------------------------------------------------------------------------- */
/* --- figure #complete 4-year periods and #remaining yrs till current --- */
nyears = year - year0;			/* #years since year0 */
if ( nyears < 0 ) return ( -1 );	/* we're not working backwards */
nfouryrs = nyears/4;			/* #complete four-year periods */
nyears -= (4*nfouryrs); 		/* remainder excluding current year*/
/* --- #days from jan 1, year0 till jan 1, this year --- */
ndays = (days4yrs*nfouryrs)		/* #days in 4-yr periods */
      +  (days1yr*nyears);		/* +remaining days */
/*if ( year > 100 ) ndays--;*/		/* subtract leap year for 2000AD */
/* --- add #days within current year --- */
ndays += (modays[month-1] + (day-1));
/* --- may need an extra day if current year is a leap year --- */
if ( nyears == 3 )			/*three preceding yrs so this is 4th*/
    { if ( month > 2 )			/* past feb so need an extra day */
	/*if ( year != 100 )*/		/* unless it's 2000AD */
	  ndays++; }			/* so add it in */
return ( (int)(ndays) );		/* #days back to caller */
} /* --- end-of-function daynumber() --- */


/* ==========================================================================
 * Function:	strwrap ( s, linelen, tablen )
 * Purpose:	Inserts \n's and spaces in (a copy of) s to wrap lines
 *		at linelen and indent them by tablen.
 * --------------------------------------------------------------------------
 * Arguments:	s (I)		char * to null-terminated string
 *				to be wrapped.
 *		linelen (I)	int containing maximum linelen
 *				between \\'s.
 *		tablen (I)	int containing number of spaces to indent
 *				lines.  0=no indent.  Positive means
 *				only indent first line and not others.
 *				Negative means indent all lines except first.
 * --------------------------------------------------------------------------
 * Returns:	( char * )	ptr to "line-wrapped" copy of s
 *				or "" (empty string) for any error.
 * --------------------------------------------------------------------------
 * Notes:     o	The returned copy of s has embedded \\'s as necessary
 *		to wrap lines at linelen.  Any \\'s in the input copy
 *		are removed first.  If (and only if) the input s contains
 *		a terminating \\ then so does the returned copy.
 *	      o	The returned pointer addresses a static buffer,
 *		so don't call strwrap() again until you're finished
 *		with output from the preceding call.
 *	      o	Modified for mimetex from original version written
 *		for mathtex (where \n in verbatim mode instead of \\
 *		produced linebreaks).
 * ======================================================================= */
/* --- entry point --- */
char	*strwrap ( char *s, int linelen, int tablen )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
static	char sbuff[4096];		/* line-wrapped copy of s */
char	*sol = sbuff;			/* ptr to start of current line*/
char	tab[32] = "                 ";	/* tab string */
int	strreplace();			/* remove \n's */
char	*strchange();			/* add \n's and indent space */
int	finalnewline = (lastchar(s)=='\n'?1:0); /*newline at end of string?*/
int	istab = (tablen>0?1:0),		/* init true to indent first line */
	iswhite = 0;			/* true if line break on whitespace*/
int	rhslen  = 0,			/* remaining right hand side length*/
	thislen = 0,			/* length of current line segment */
	thistab = 0,			/* length of tab on current line */
	wordlen = 0;			/* length to next whitespace char */
/* -------------------------------------------------------------------------
Make a clean copy of s
-------------------------------------------------------------------------- */
/* --- check input --- */
*sbuff = '\000';			/* initialize in case of error */
if ( isempty(s) ) goto end_of_job;	/* no input */
if ( tablen < 0 ) tablen = (-tablen);	/* set positive tablen */
if ( tablen >= linelen ) tablen = linelen-1; /* tab was longer than line */
tab[min2(tablen,16)] = '\000';		/* null-terminate tab string */
tablen = strlen(tab);			/* reset to actual tab length */
finalnewline = 0;			/* turned off for mimetex version */
/* --- start with copy of s --- */
strninit(sbuff,s,3000);			/* leave room for \n's and tabs */
if ( linelen < 1 ) goto end_of_job;	/* can't do anything */
trimwhite(sbuff);			/*remove leading/trailing whitespace*/
strreplace(sbuff,"\n"," ",0);		/* remove any original \n's */
strreplace(sbuff,"\r"," ",0);		/* remove any original \r's */
strreplace(sbuff,"\t"," ",0);		/* remove any original \t's */
strreplace(sbuff,"\f"," ",0);		/* remove any original \f's */
strreplace(sbuff,"\v"," ",0);		/* remove any original \v's */
strreplace(sbuff,"\\\\"," ",0);		/* remove any original \\'s */
/* -------------------------------------------------------------------------
Insert \\'s and spaces as needed
-------------------------------------------------------------------------- */
while ( 1 ) {				/* till end-of-line */
  /* --- init --- */
  trimwhite(sol);			/*remove leading/trailing whitespace*/
  thislen = thistab = 0;		/* no chars in current line yet */
  if ( istab && tablen>0 ) {		/* need to indent this line */
    strchange(0,sol,tab);		/* insert indent at start of line */
    thistab = tablen; }			/* line starts with whitespace tab */
  if ( sol == sbuff ) istab = 1-istab;	/* flip tab flag after first line */
  sol += thistab;			/* skip tab */
  rhslen = strlen(sol);			/* remaining right hand side chars */
  if ( rhslen+thistab <= linelen ) break; /* no more \\'s needed */
  if ( 0 && msgfp!=NULL && msglevel >= 99 ) {
    fprintf(msgfp,"strwrap> rhslen=%d, sol=\"\"%s\"\"\n",rhslen,sol);
    fflush(msgfp); }
  /* --- look for last whitespace preceding linelen --- */
  while ( 1 ) {				/* till we exceed linelen */
    wordlen = strcspn(sol+thislen," \t\n\r\f\v :;.,"); /*ptr to next white/break*/
    if ( sol[thislen+wordlen] == '\000' ) /* no more whitespace in string */
      goto end_of_job;			/* so nothing more we can do */
    if ( thislen+thistab+wordlen >= linelen ) /* next word won't fit */
      if ( thislen > 0 ) break;		/* but make sure line has one word */
    thislen += (wordlen+1); }		/* ptr past next whitespace char */
  if ( thislen < 1 ) break;		/* line will have one too-long word*/
  /*sol[thislen-1] = '\n';*/		/* replace last space with newline */
  /*sol += thislen;*/			/* next line starts after newline */
  iswhite = (isthischar(sol[thislen-1],":;.,")?0:1); /*linebreak on space?*/
  strchange(iswhite,sol+thislen-iswhite,"\\\\"); /* put \\ at end of line */
  sol += (thislen+2-iswhite);		/* next line starts after \\ */
  } /* --- end-of-while(1) --- */
end_of_job:
  if ( finalnewline ) strcat(sbuff,"\\\\"); /* replace final newline */
  return ( sbuff );			/* back with clean copy of s */
} /* --- end-of-function strwrap() --- */


/* ==========================================================================
 * Function:	strnlower ( s, n )
 * Purpose:	lowercase the first n chars of string s
 * --------------------------------------------------------------------------
 * Arguments:	s (I/O)		(char *)pointer to null-terminated string
 *				whose chars are to be lowercased
 *		n (I)		int containing max number of chars to be
 *				lowercased (less than n will be lowercased
 *				if terminating '\000' found first)
 *				If n<=0 (or n>=strlen(s)) then the entire
 *				string s will be lowercased
 * --------------------------------------------------------------------------
 * Returns:	( char * )	s (always same as input)
 * --------------------------------------------------------------------------
 * Notes:     o
 * ======================================================================= */
/* --- entry point --- */
char	*strnlower ( char *s, int n )
{
/* -------------------------------------------------------------------------
lowercase s
-------------------------------------------------------------------------- */
char	*p = s;				/* save s for return to caller */
if ( !isempty(s) )			/* check for valid input */
  while ( *p != '\000' ) {		/* lowercase each char till end */
    *p = tolower(*p);			/* lowercase this char */
    if ( n > 0 )			/* only lowercase first n chars */
      if ( --n < 1 ) break;		/* quit when we're done */
    p++; }				/* proceed to next char */
return ( s );				/* back to caller with s */
} /* --- end-of-function strnlower() --- */


/* ==========================================================================
 * Function:	urlprune ( url, n )
 * Purpose:	Prune http://abc.def.ghi.com/etc into abc.def.ghi.com
 *		(if n=2 only ghi.com is returned, or if n=-1 only "ghi")
 * --------------------------------------------------------------------------
 * Arguments:	url (I)		char * to null-terminated string
 *				containing url to be pruned
 *		n (i)		int containing number of levels retained
 *				in pruned url.  If n<0 its abs() is used,
 *				but the topmost level (usually .com, .org,
 *				etc) is omitted.  That is, if n=2 would
 *				return "ghi.com" then n=-1 returns "ghi".
 *				n=0 retains all levels.
 * --------------------------------------------------------------------------
 * Returns:	( char * )	pointer to (static) null-terminated string
 *				containing pruned url with the first n
 *				top-level domain, e.g., for n=2,
 *				http://abc.def.ghi.com/etc returns ghi.com,
 *				or an empty string "\000" for any error
 * --------------------------------------------------------------------------
 * Notes:     o
 * ======================================================================= */
/* --- entry point --- */
char	*urlprune ( char *url, int n )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
static	char pruned[2048];		/* pruned url returned to caller */
char	*purl = /*NULL*/pruned;		/* ptr to pruned, init for error */
char	*delim = NULL;			/* delimiter separating components */
char	*strnlower();			/* lowercase a string */
int	istruncate = (n<0?1:0);		/*true to truncate .com from pruned*/
int	ndots = 0;			/* number of dots found in url */
/* -------------------------------------------------------------------------
prune the url
-------------------------------------------------------------------------- */
/* --- first check input --- */
*pruned = '\000';			/* init for error */
if ( isempty(url) ) goto end_of_job;	/* missing input, so return NULL */
if ( n < 0 ) n = (-n);			/* flip n positive */
if ( n == 0 ) n = 999;			/* retain all levels of url */
/* --- preprocess url --- */
strninit(pruned,url,2032);		/* copy url to our static buffer */
strlower(pruned);			/* lowercase it and... */
trimwhite(pruned);			/*remove leading/trailing whitespace*/
/* --- first remove leading http:// --- */
if ( (delim=strstr(pruned,"://")) != NULL ) /* found http:// or ftp:// etc */
  if ( ((int)(delim-pruned)) <= 8 ) {	/* make sure it's a prefix */
    strsqueezep(pruned,delim+3);	/* squeeze out leading http:// */
    trimwhite(pruned); }		/*remove leading/trailing whitespace*/
/* --- next remove leading www. --- */
if ( (delim=strstr(pruned,"www.")) != NULL ) /* found www. */
  if ( ((int)(delim-pruned)) == 0 ) {	/* make sure it's the leading chars*/
    strsqueezep(pruned,delim+4);	/* squeeze out leading www. */
    trimwhite(pruned); }		/*remove leading/trailing whitespace*/
/* --- finally remove leading / and everything following it --- */
if ( (delim=strchr(pruned,'/')) != NULL ) /* found first / */
  *delim = '\000';			/* null-terminate url at first / */
if ( isempty(pruned) ) goto end_of_job;	/* nothing left in url */
/* --- count dots from back of url --- */
delim = pruned + strlen(pruned);	/*ptr to '\000' terminating pruned*/
while ( ((int)(delim-pruned)) > 0 ) {	/* don't back up before first char */
  delim--;				/* ptr to preceding character */
  if ( *delim != '.' ) continue;	/* not a dot, so keep looking */
  ndots++;				/* count another dot found */
  if ( istruncate ) {			/* remove trailing .com */
    istruncate = 0;			/* don't truncate any more dots */
    *delim = '\000';			/* truncate pruned url */
    ndots = 0; }			/* and reset dot count */
  if ( ndots >= n ) {			/* have all requested levels */
    strsqueezep(pruned,delim+1);	/* squeeze out leading levels */
    break; }				/* and we're done */
  } /* --- end-of-while() --- */
purl = pruned;				/*completed okay, return pruned url*/
end_of_job:
  return ( purl );			/* back with pruned url */
} /* --- end-of-function urlprune() --- */


/* ==========================================================================
 * Function:	urlncmp ( url1, url2, n )
 * Purpose:	Compares the n topmost levels of two urls
 * --------------------------------------------------------------------------
 * Arguments:	url1 (I)	char * to null-terminated string
 *				containing url to be compared with url2
 *		url2 (I)	char * to null-terminated string
 *				containing url to be compared with url1
 *		n (I)		int containing number of top levels
 *				to compare, or 0 to compare them all.
 *				n<0 compares that many top levels excluding
 *				the last, i.e., for n=-1, xxx.com and xxx.org
 *				would be considered a match
 * --------------------------------------------------------------------------
 * Returns:	( int )		1 if url's match, or
 *				0 if not.
 * --------------------------------------------------------------------------
 * Notes:     o
 * ======================================================================= */
/* --- entry point --- */
int	urlncmp ( char *url1, char *url2, int n )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
char	*urlprune(), *prune=NULL,	/* prune url's */
	prune1[4096], prune2[4096];	/* pruned copies of url1,url2 */
int	ismatch = 0;			/* true if url's match */
/* -------------------------------------------------------------------------
prune url's and compare the pruned results
-------------------------------------------------------------------------- */
/* --- check input --- */
if ( isempty(url1)			/*make sure both url1,url2 supplied*/
||   isempty(url2) ) goto end_of_job;	/* missing input, so return 0 */
/* --- prune url's --- */
prune = urlprune(url1,n);		/* ptr to pruned version of url1 */
if ( isempty(prune) ) goto end_of_job;	/* some problem with url1 */
strninit(prune1,prune,4064);		/* local copy of pruned url1 */
prune = urlprune(url2,n);		/* ptr to pruned version of url2 */
if ( isempty(prune) ) goto end_of_job;	/* some problem with url2 */
strninit(prune2,prune,4064);		/* local copy of pruned url2 */
/* --- compare pruned url's --- */
if ( strcmp(prune1,prune2) == 0 )	/* pruned url's are identical */
  ismatch = 1;				/* signal match to caller */
end_of_job:
  return ( ismatch );			/*back with #matching url components*/
} /* --- end-of-function urlncmp() --- */


/* ==========================================================================
 * Function:	dbltoa ( dblval, npts )
 * Purpose:	Converts double to ascii, in financial format
 *		(e.g., comma-separated and negatives enclosed in ()'s).
 * -------------------------------------------------------------------------
 * Arguments:	dblval (I)	double containing value to be converted.
 *		npts (I)	int containing #places after decimal point
 *				to be displayed in returned string.
 * Returns:	( char * )	null-terminated string containing
 *				double converted to financial format.
 * -------------------------------------------------------------------------
 * Notes:     o
 * ======================================================================= */
/* --- entry point --- */
char	*dbltoa ( double dblval, int npts )
/* double dblval;
   int	npts; */
{
/* -------------------------------------------------------------------------
Allocations and Declarations
------------------------------------------------------------------------- */
static	char finval[256];		/* buffer returned to caller */
static	char digittbl[32]="0123456789*"; /* table of ascii decimal digits */
char	*finptr = finval;		/* ptr to next char being converted*/
double	floor();			/* integer which is glb(double) */
double	dbldigit;			/* to shift out digits from dblval */
int	digit;				/* one digit from dblval */
int	isneg = 0;			/* reset true if dblval negative */
int	ifrac = 0;			/* npts fractional digits of dblval*/
char	digits[64]; int ndigits=0;	/* all the digits [0]=least signif */
/* -------------------------------------------------------------------------
Check sign
------------------------------------------------------------------------- */
if ( dblval < 0.0 )			/* got a negative value to convert */
    { isneg=1; dblval=(-dblval); }	/* set flag and make it positive */
/* -------------------------------------------------------------------------
Get fractional part of dblval if required
------------------------------------------------------------------------- */
if ( npts > 0 )
    { int ipts = npts;			/* loop index */
      dbldigit = dblval-floor(dblval);	/* fractional part as double */
      digit = 1;			/* check if rounded frac > 1 */
      while ( --ipts >= 0 )		/* count down */
	{ dbldigit *= 10.0;		/* shift left one digit at a time */
	  digit *= 10; }		/* and keep max up-to-date */
      ifrac = (int)(dbldigit + 0.5);	/* store fractional part as integer*/
      if ( ifrac >= digit )		/* round to next whole number */
	{ dblval++; ifrac=0; }		/* bump val, reset frac to zero */
    } /* --- end-of-if(npts>0) --- */
else dblval += 0.5;			/* no frac, round to nearest whole */
/* -------------------------------------------------------------------------
Get whole digits
------------------------------------------------------------------------- */
dblval = floor(dblval);			/* get rid of fractional part */
while ( dblval > 0.0 )			/* still have data digits remaining*/
    { dbldigit = floor(dblval/10.0);	/* shift out next digit */
      digit = (int)(dblval - 10.0*dbldigit + 0.01); /* least signif digit */
      if ( digit<0 || digit>9 ) digit=10; /* index check */
      digits[ndigits++] = digittbl[digit]; /* store ascii digit */
      dblval = dbldigit; }		/* ready for next digit */
if ( ndigits < 1 ) digits[ndigits++] = '0'; /* store a single '0' for 0.0 */
/* -------------------------------------------------------------------------
Format whole part from digits[] array
------------------------------------------------------------------------- */
if ( isneg ) *finptr++ = '(';		/* leading paren for negative value*/
for ( digit=ndigits-1; digit>=0; digit-- ) /* start with most significant */
    { *finptr++ = digits[digit];	/* store digit */
      if ( digit>0 && digit%3==0 )	/* need a comma */
	*finptr++ = ','; }		/* put in separating comma */
/* -------------------------------------------------------------------------
Format fractional part using ifrac
------------------------------------------------------------------------- */
if ( npts > 0 )
    { *finptr++ = '.';			/* start with decimal point */
      sprintf(finptr,"%0*d",npts,ifrac); /* convert to string */
      finptr += npts; }			/* bump ptr past fractional digits */
/* -------------------------------------------------------------------------
End-of-Job
------------------------------------------------------------------------- */
if ( isneg ) *finptr++ = ')';		/*trailing paren for negative value*/
*finptr = '\000';			/* null-terminate converted double */
return ( finval );			/* converted double back to caller */
} /* --- end-of-function dbltoa() --- */


/* ==========================================================================
 * Function:	rotmatrix ( axis, theta )
 *		Constructs rotation matrix for theta degrees around axis
 *		(see Notes below for discussion).
 * --------------------------------------------------------------------------
 * Arguments:	axis (I)	point3d * to u=(u_x,u_y,u_z) components
 *				of rotation axis (u's tail point at origin),
 *				or NULL ptr returns previous matrix
 *		theta (I)	double containing rotation in degrees
 *				(positive theta rotation according to
 *				right-hand screw rule),
 *				or 0.0 returns previous matrix
 * --------------------------------------------------------------------------
 * Returns:	( matrix3d * )	pointer to constructed rotation matrix,
 *				or NULL for any error
 * --------------------------------------------------------------------------
 * Notes:     o	For discussion, see
 *		http://wikipedia.org/wiki/Rotation_matrix#In_three_dimensions
 *		
 * ======================================================================= */
/* --- entry point --- */
matrix3d *rotmatrix ( point3d *axis, double theta )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
static	matrix3d rot =			/* returned rotation matrix */
	{ {0.,0.,0,}, {0.,0.,0,}, {0.,0.,0,} }; /* just init as zero's */
matrix3d *prot = &(rot);		/* init returned rot ptr */
double	pi = 3.14159265359;		/* pi */
double	u,ux,uy,uz,			/* axis length and components */
	tsin,tcos;			/* sin,cos (theta) */
/* -------------------------------------------------------------------------
Initialization
-------------------------------------------------------------------------- */
if ( axis == NULL			/*  no rotation axis, or no theta */
||   absval(theta) < 1.0e-10 ) goto end_of_job;/*just return preceding matrix*/
if ( 1 ) theta *= (pi/180.);		/*convert theta in degrees to radians*/
tsin = sin(theta);  tcos = cos(theta);	/* sin,cos (theta) */
ux = axis->x;  uy = axis->y;  uz = axis->z; /* unnormalized axis components */
u = sqrt((ux*ux)+(uy*uy)+(uz*uz));	/* axis length */
if ( u < 1.0e-10 ) goto end_of_job;	/* zero-vector is an error */
ux /= u;  uy /= u;  uz /= u;		/* normalized axis components */
/* -------------------------------------------------------------------------
rotation matrix components, stored row-wise
-------------------------------------------------------------------------- */
/* --- x-row --- */
rot.xrow.x = tcos + ux*ux*(1.0-tcos);
rot.xrow.y =        ux*uy*(1.0-tcos) - uz*tsin;
rot.xrow.z =        ux*uz*(1.0-tcos) + uy*tsin;
/* --- y-row --- */
rot.yrow.x =        uy*ux*(1.0-tcos) + uz*tsin;
rot.yrow.y = tcos + uy*uy*(1.0-tcos);
rot.yrow.z =        uy*uz*(1.0-tcos) - ux*tsin;
/* --- z-row --- */
rot.zrow.x =        uz*ux*(1.0-tcos) - uy*tsin;
rot.zrow.y =        uz*uy*(1.0-tcos) + ux*tsin;
rot.zrow.z = tcos + uz*uz*(1.0-tcos);
prot = &(rot);				/* point return value to matrix */
end_of_job:
  return ( prot );			/*back to caller with rotation matrix*/
} /* --- end-of-function rotmatrix() --- */


/* ==========================================================================
 * Function:	matmult ( mat, vec )
 *		returns result of mat(rix) x vec(tor)
 * --------------------------------------------------------------------------
 * Arguments:	mat (I)		matrix3d * to matrix
 *		vec (I)		point3d *  to vector
 * --------------------------------------------------------------------------
 * Returns:	( point3d * )	pointer to result of mat(rix) x vec(tor),
 *				or NULL for any error
 * --------------------------------------------------------------------------
 * Notes:     o
 * ======================================================================= */
/* --- entry point --- */
point3d	*matmult ( matrix3d *mat, point3d *vec )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
static	point3d mult;			/* returned matrix x vector product */
point3d *pmult = NULL;			/* init mult ptr for error */
/* -------------------------------------------------------------------------
Initialization
-------------------------------------------------------------------------- */
if ( mat == NULL			/* mat and vec required input */
||   vec == NULL ) goto end_of_job;	/* missing required input */
/* -------------------------------------------------------------------------
matrix multiplication
-------------------------------------------------------------------------- */
/* --- x,y,z-components of mat(rix) x vec(tor) product --- */
mult.x = (mat->xrow.x)*vec->x + (mat->xrow.y)*vec->y + (mat->xrow.z)*vec->z;
mult.y = (mat->yrow.x)*vec->x + (mat->yrow.y)*vec->y + (mat->yrow.z)*vec->z;
mult.z = (mat->zrow.x)*vec->x + (mat->zrow.y)*vec->y + (mat->zrow.z)*vec->z;
pmult = &(mult);			/* point return value to product */
end_of_job:
  return ( pmult );			/*back to caller with matrix x vector*/
} /* --- end-of-function matmult() --- */


/* ==========================================================================
 * Function:	aalowpass ( rp, bytemap, grayscale )
 * Purpose:	calculates a lowpass anti-aliased bytemap
 *		for rp->bitmap, with each byte 0...grayscale-1
 * --------------------------------------------------------------------------
 * Arguments:	rp (I)		raster *  to raster whose bitmap
 *				is to be anti-aliased
 *		bytemap (O)	intbyte * to bytemap, calculated
 *				by applying lowpass filter to rp->bitmap,
 *				and returned (as you'd expect) in 1-to-1
 *				addressing correspondence with rp->bitmap
 *		grayscale (I)	int containing number of grayscales
 *				to be calculated, 0...grayscale-1
 *				(should typically be given as 256)
 * --------------------------------------------------------------------------
 * Returns:	( int )		1=success, 0=any error
 * --------------------------------------------------------------------------
 * Notes:     o	If the center point of the box being averaged is black,
 *		then the entire "average" is forced black (grayscale-1)
 *		regardless of the surrounding points.  This is my attempt
 *		to avoid a "washed-out" appearance of thin (one-pixel-wide)
 *		lines, which would otherwise turn from black to a gray shade.
 *	     o	Also, while the weights for neighbor points are fixed,
 *		you may adjust the center point weight on the compile line.
 *		A higher weight sharpens the resulting anti-aliased image;
 *		lower weights blur it out more (but keep the "center" black
 *		as per the preceding note).
 * ======================================================================= */
/* --- entry point --- */
int	aalowpass (raster *rp, intbyte *bytemap, int grayscale)
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
int	status = 1;			/* 1=success, 0=failure to caller */
pixbyte	*bitmap= (rp==NULL?NULL:rp->pixmap); /*local rp->pixmap ptr*/
int	irow=0, icol=0;			/* rp->height, rp->width indexes */
int	weights[9] = { 1,3,1, 3,0,3, 1,3,1 }; /* matrix of weights */
int	adjindex[9]= { 0,1,2, 7,-1,3, 6,5,4 }; /*clockwise from upper-left*/
int	totwts = 0;			/* sum of all weights in matrix */
int	isforceavg = 1,			/*force avg black if center black?*/
	isminmaxwts = 1,		/*use wts or #pts for min/max test */
	blackscale = 0; /*(grayscale+1)/4;*/ /*force black if wgted avg>bs */
/* -------------------------------------------------------------------------
Initialization
-------------------------------------------------------------------------- */
/* --- calculate total weights --- */
weights[4]= centerwt;			/* weight for center point */
weights[1]= weights[3]= weights[5]= weights[7]= adjacentwt; /*adjacent pts*/
totwts = centerwt + 4*(1+adjacentwt);	/* tot is center plus neighbors */
/* -------------------------------------------------------------------------
Calculate bytemap as 9-point weighted average over bitmap
-------------------------------------------------------------------------- */
for ( irow=0; irow<rp->height; irow++ )
 for ( icol=0; icol<rp->width; icol++ )
  {
  int	ipixel = icol + irow*(rp->width); /* center pixel index */
  int	jrow=0, jcol=0,			/* box around ipixel */
	bitval = 0,			/* value of bit/pixel at jrow,jcol */
	iscenter = 0,			/* set true if center pixel black */
	nadjacent=0, wadjacent=0,	/* #adjacent black pixels, their wts*/
	ngaps = 0,			/* #gaps in 8 pixels around center */
	iwt=(-1), sumwts=0;		/* weights index, sum in-bound wts */
  char	adjmatrix[8];			/* adjacency "matrix" */
  memset(adjmatrix,0,8);		/* zero out adjacency matrix */
  bytemap[ipixel] = 0;			/* init pixel white */
  /*--- for ipixel at irow,icol, get weighted average of adjacent pixels ---*/
  for ( jrow=irow-1; jrow<=irow+1; jrow++ )  /* jrow = irow-1...irow+1 */
   for ( jcol=icol-1; jcol<=icol+1; jcol++ ) /* jcol = icol-1...icol+1 */
    {
    int	jpixel = jcol + jrow*(rp->width); /* averaging index */
    iwt++;				/*always bump weight index*/
    if ( jrow<0 || jrow>=rp->height	/* if row out pf bounds */
    ||   jcol<0 || jcol>=rp->width )	/* or col out of bounds */
	continue;			/* ignore this point */
    bitval = (int)getlongbit(bitmap,jpixel); /* value of bit at jrow,jcol */
    if ( bitval )			/* this is a black pixel */
      {	if ( jrow==irow && jcol==icol )	/* and this is center point */
	  iscenter = 1;			/* set flag for center point black */
	else				/* adjacent point black */
	  { nadjacent++;		/* bump adjacent black count */
	    adjmatrix[adjindex[iwt]] = 1; } /*set "bit" in adjacency matrix*/
	wadjacent += weights[iwt]; }	/* sum weights for black pixels */
    sumwts += weights[iwt];		/* and sum weights for all pixels */
    } /* --- end-of-for(jrow,jcol) --- */
  /* --- count gaps --- */
  ngaps = (adjmatrix[7]!=adjmatrix[0]?1:0); /* init count */
  for ( iwt=0; iwt<7; iwt++ )		/* clockwise around adjacency */
    if ( adjmatrix[iwt] != adjmatrix[iwt+1] ) ngaps++; /* black/white flip */
  ngaps /= 2;				/*each gap has 2 black/white flips*/
  /* --- anti-alias pixel, but leave it black if it was already black --- */
  if ( isforceavg && iscenter )		/* force avg if center point black */
      bytemap[ipixel] = grayscale-1;	/* so force grayscale-1=black */
  else					/* center point not black */
   if ( ngaps <= 2 )			/*don't darken checkerboarded pixel*/
    { bytemap[ipixel] =			/* 0=white ... grayscale-1=black */
	((totwts/2 - 1) + (grayscale-1)*wadjacent)/totwts; /* not /sumwts; */
      if ( blackscale > 0		/* blackscale kludge turned on */
      &&   bytemap[ipixel] > blackscale ) /* weighted avg > blackscale */
	bytemap[ipixel] = grayscale-1; } /* so force it entirely black */
  /*--- only anti-alias pixels whose adjacent pixels fall within bounds ---*/
  if ( !iscenter ) {			/* apply min/maxadjacent test */
   if ( isminmaxwts )			/* min/max refer to adjacent weights*/
    { if ( wadjacent < minadjacent	/* wts of adjacent points too low */
      ||   wadjacent > maxadjacent )	/* or too high */
	bytemap[ipixel] = 0; }		/* so leave point white */
   else					/* min/max refer to #adjacent points*/
    { if ( nadjacent < minadjacent	/* too few adjacent points black */
      ||   nadjacent > maxadjacent )	/* or too many */
	bytemap[ipixel] = 0; } }	/* so leave point white */
  } /* --- end-of-for(irow,icol) --- */
/* -------------------------------------------------------------------------
Back to caller with gray-scale anti-aliased bytemap
-------------------------------------------------------------------------- */
/*end_of_job:*/
  return ( status );
} /* --- end-of-function aalowpass() --- */


/* ==========================================================================
 * Function:	aapnm ( rp, bytemap, grayscale )
 * Purpose:	calculates a lowpass anti-aliased bytemap
 *		for rp->bitmap, with each byte 0...grayscale-1,
 *		based on the pnmalias.c algorithm
 * --------------------------------------------------------------------------
 * Arguments:	rp (I)		raster *  to raster whose bitmap
 *				is to be anti-aliased
 *		bytemap (O)	intbyte * to bytemap, calculated
 *				by applying pnm-based filter to rp->bitmap,
 *				and returned (as you'd expect) in 1-to-1
 *				addressing correspondence with rp->bitmap
 *		grayscale (I)	int containing number of grayscales
 *				to be calculated, 0...grayscale-1
 *				(should typically be given as 256)
 * --------------------------------------------------------------------------
 * Returns:	( int )		1=success, 0=any error
 * --------------------------------------------------------------------------
 * Notes:    o	Based on the pnmalias.c algorithm in the netpbm package
 *		on sourceforge.
 * ======================================================================= */
/* --- entry point --- */
int	aapnm (raster *rp, intbyte *bytemap, int grayscale)
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
pixbyte	*bitmap = rp->pixmap;		/* local rp->pixmap ptr */
int	width=rp->width, height=rp->height, /* width, height of raster */
	icol = 0,        irow = 0,	/* width, height indexes */
	imap = (-1);			/* pixel index = icol + irow*width */
int	bgbitval=0, fgbitval=1;		/* background, foreground bitval */
int	isfirstaa = 1;			/*debugging switch signals 1st pixel*/
#if 0
int	totwts=12, wts[9]={1,1,1, 1,4,1, 1,1,1}; /* pnmalias default wts */
int	totwts=16, wts[9]={1,2,1, 2,4,2, 1,2,1}; /* weights */
#endif
int	totwts=18, wts[9]={1,2,1, 2,6,2, 1,2,1}; /* pnmalias default wts */
int	isresetparams = 1,		/* true to set antialiasing params */
	isfgalias  = 1,			/* true to antialias fg bits */
	isfgonly   = 0,			/* true to only antialias fg bits */
	isbgalias  = 0,			/* true to antialias bg bits */
	isbgonly   = 0;			/* true to only antialias bg bits */
/* -------------------------------------------------------------------------
Initialization
-------------------------------------------------------------------------- */
/* --- check for bold light --- */
if ( 0 )
 { if ( weightnum > 2 ) { isbgalias=1; }	/* simulate bold */
   if ( weightnum < 1 ) { isbgonly=1; isfgalias=0; } } /* simulate light */
/* --- reset wts[], etc, and calculate total weights --- */
if ( isresetparams )			/* wts[], etc taken from params */
  { int	iwt=0;				/* wts[iwt] index */
    wts[4]= centerwt;			/* weight for center point */
    wts[1]=wts[3]=wts[5]=wts[7] = adjacentwt; /* and adjacent points */
    wts[0]=wts[2]=wts[6]=wts[8] = cornerwt;   /* and corner points */
    for ( totwts=0,iwt=0; iwt<9; iwt++ ) totwts += wts[iwt]; /* sum wts */
    isfgalias = fgalias;		/* set isfgalias */
    isfgonly = fgonly;			/* set isfgonly */
    isbgalias = bgalias;		/* set isbgalias */
    isbgonly = bgonly; }		/* set isbgonly */
/* -------------------------------------------------------------------------
Calculate bytemap as 9-point weighted average over bitmap
-------------------------------------------------------------------------- */
for ( irow=0; irow<height; irow++ )
 for ( icol=0; icol<width; icol++ )
  {
  /* --- local allocations and declarations --- */
  int	bitval=0,			/* value of rp bit at irow,icol */
	nnbitval=0, nebitval=0, eebitval=0, sebitval=0,	/*adjacent vals*/
	ssbitval=0, swbitval=0, wwbitval=0, nwbitval=0;	/*compass pt names*/
  int	isbgedge=0, isfgedge=0;		/*does pixel border a bg or fg edge*/
  int	aabyteval=0;			/* antialiased (or unchanged) value*/
  /* --- bump imap index and get center bit value --- */
  imap++;				/* imap = icol + irow*width */
  bitval = getlongbit(bitmap,imap);	/* value of rp input bit at imap */
  aabyteval = (intbyte)(bitval==bgbitval?0:grayscale-1); /* default aa val */
  bytemap[imap] = (intbyte)(aabyteval);	/* init antialiased pixel */
  /* --- check if we're antialiasing this pixel --- */
  if ( (isbgonly && bitval==fgbitval)	/* only antialias background bit */
  ||   (isfgonly && bitval==bgbitval) )	/* only antialias foreground bit */
    continue;				/* leave default and do next bit */
  /* --- get surrounding bits --- */
  if ( irow > 0 )			/* nn (north) bit available */
     nnbitval = getlongbit(bitmap,imap-width); /* nn bit value */
  if ( irow < height-1 )		/* ss (south) bit available */
     ssbitval = getlongbit(bitmap,imap+width); /* ss bit value */
  if ( icol > 0 )			/* ww (west) bit available */
   { wwbitval = getlongbit(bitmap,imap-1); /* ww bit value */
     if ( irow > 0 )			/* nw bit available */
       nwbitval = getlongbit(bitmap,imap-width-1); /* nw bit value */
     if ( irow < height-1 )		/* sw bit available */
       swbitval = getlongbit(bitmap,imap+width-1); } /* sw bit value */
  if ( icol < width-1 )			/* ee (east) bit available */
   { eebitval = getlongbit(bitmap,imap+1); /* ee bit value */
     if ( irow > 0 )			/* ne bit available */
       nebitval = getlongbit(bitmap,imap-width+1); /* ne bit value */
     if ( irow < height-1 )		/* se bit available */
       sebitval = getlongbit(bitmap,imap+width+1); } /* se bit value */
  /* --- check for edges --- */
  isbgedge =				/* current pixel borders a bg edge */
	(nnbitval==bgbitval && eebitval==bgbitval) ||	/*upper-right edge*/
	(eebitval==bgbitval && ssbitval==bgbitval) ||	/*lower-right edge*/
	(ssbitval==bgbitval && wwbitval==bgbitval) ||	/*lower-left  edge*/
	(wwbitval==bgbitval && nnbitval==bgbitval) ;	/*upper-left  edge*/
  isfgedge =				/* current pixel borders an fg edge*/
	(nnbitval==fgbitval && eebitval==fgbitval) ||	/*upper-right edge*/
	(eebitval==fgbitval && ssbitval==fgbitval) ||	/*lower-right edge*/
	(ssbitval==fgbitval && wwbitval==fgbitval) ||	/*lower-left  edge*/
	(wwbitval==fgbitval && nnbitval==fgbitval) ;	/*upper-left  edge*/
  /* ---check top/bot left/right edges for corners (added by j.forkosh)--- */
  if ( 1 ) {				/* true to perform test */
    int	isbghorz=0, isfghorz=0, isbgvert=0, isfgvert=0; /* horz/vert edges */
    isbghorz =				/* top or bottom edge is all bg */
	(nwbitval+nnbitval+nebitval == 3*bgbitval) ||	/* top edge bg */
	(swbitval+ssbitval+sebitval == 3*bgbitval) ;	/* bottom edge bg */
    isfghorz =				/* top or bottom edge is all fg */
	(nwbitval+nnbitval+nebitval == 3*fgbitval) ||	/* top edge fg */
	(swbitval+ssbitval+sebitval == 3*fgbitval) ;	/* bottom edge fg */
    isbgvert =				/* left or right edge is all bg */
	(nwbitval+wwbitval+swbitval == 3*bgbitval) ||	/* left edge bg */
	(nebitval+eebitval+sebitval == 3*bgbitval) ;	/* right edge bg */
    isfgvert =				/* left or right edge is all bg */
	(nwbitval+wwbitval+swbitval == 3*fgbitval) ||	/* left edge fg */
	(nebitval+eebitval+sebitval == 3*fgbitval) ;	/* right edge fg */
    if ( (isbghorz && isbgvert && (bitval==fgbitval))	/* we're at an...*/
    ||   (isfghorz && isfgvert && (bitval==bgbitval)) )	/*...inside corner */
	continue;					/* don't antialias */
    } /* --- end-of-if(1) --- */
  /* --- check #gaps for checkerboard (added by j.forkosh) --- */
  if ( 0 ) {				/* true to perform test */
    int	ngaps=0, mingaps=1,maxgaps=2;	/* count #fg/bg flips (max=4 noop) */
    if ( nwbitval!=nnbitval ) ngaps++;	/* upper-left =? upper */
    if ( nnbitval!=nebitval ) ngaps++;	/* upper =? upper-right */
    if ( nebitval!=eebitval ) ngaps++;	/* upper-right =? right */
    if ( eebitval!=sebitval ) ngaps++;	/* right =? lower-right */
    if ( sebitval!=ssbitval ) ngaps++;	/* lower-right =? lower */
    if ( ssbitval!=swbitval ) ngaps++;	/* lower =? lower-left */
    if ( swbitval!=wwbitval ) ngaps++;	/* lower-left =? left */
    if ( wwbitval!=nwbitval ) ngaps++;	/* left =? upper-left */
    if ( ngaps > 0 ) ngaps /= 2;	/* each gap has 2 bg/fg flips */
    if ( ngaps<mingaps || ngaps>maxgaps ) continue;
    } /* --- end-of-if(1) --- */
  /* --- antialias if necessary --- */
  if ( (isbgalias && isbgedge)		/* alias pixel surrounding bg */
  ||   (isfgalias && isfgedge)		/* alias pixel surrounding fg */
  ||   (isbgedge  && isfgedge) )	/* neighboring fg and bg pixel */
    {
    int	aasumval =			/* sum wts[]*bitmap[] */
	wts[0]*nwbitval + wts[1]*nnbitval + wts[2]*nebitval +
	wts[3]*wwbitval +  wts[4]*bitval  + wts[5]*eebitval +
	wts[6]*swbitval + wts[7]*ssbitval + wts[8]*sebitval ;
    double aawtval = ((double)aasumval)/((double)totwts); /* weighted val */
    aabyteval= (int)(((double)(grayscale-1))*aawtval+0.5); /*0...grayscale-1*/
    bytemap[imap] = (intbyte)(aabyteval); /* set antialiased pixel */
    if ( msglevel>=99 && msgfp!=NULL ) { fprintf(msgfp, /*diagnostic output*/
      "%s> irow,icol,imap=%d,%d,%d aawtval=%.4f aabyteval=%d\n",
      (isfirstaa?"aapnm algorithm":"aapnm"),
      irow,icol,imap, aawtval,aabyteval);
      isfirstaa = 0; }
    } /* --- end-of-if(isedge) --- */
  } /* --- end-of-for(irow,icol) --- */
/* -------------------------------------------------------------------------
Back to caller with gray-scale anti-aliased bytemap
-------------------------------------------------------------------------- */
/*end_of_job:*/
  return ( 1 );
} /* --- end-of-function aapnm() --- */


/* ==========================================================================
 * Function:	aapnmlookup ( rp, bytemap, grayscale )
 * Purpose:	calculates a lowpass anti-aliased bytemap
 *		for rp->bitmap, with each byte 0...grayscale-1,
 *		based on the pnmalias.c algorithm.
 *		This version uses aagridnum() and aapatternnum() lookups
 *		to interpret 3x3 lowpass pixel grids.
 * --------------------------------------------------------------------------
 * Arguments:	rp (I)		raster *  to raster whose bitmap
 *				is to be anti-aliased
 *		bytemap (O)	intbyte * to bytemap, calculated
 *				by applying pnm-based filter to rp->bitmap,
 *				and returned (as you'd expect) in 1-to-1
 *				addressing correspondence with rp->bitmap
 *		grayscale (I)	int containing number of grayscales
 *				to be calculated, 0...grayscale-1
 *				(should typically be given as 256)
 * --------------------------------------------------------------------------
 * Returns:	( int )		1=success, 0=any error
 * --------------------------------------------------------------------------
 * Notes:    o	Based on the pnmalias.c algorithm in the netpbm package
 *		on sourceforge.
 *	     o	This version uses aagridnum() and aapatternnum() lookups
 *		to interpret 3x3 lowpass pixel grids.
 * ======================================================================= */
/* --- entry point --- */
int	aapnmlookup (raster *rp, intbyte *bytemap, int grayscale)
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
int	width=rp->width, height=rp->height, /* width, height of raster */
	icol = 0,        irow = 0,	/* width, height indexes */
	imap = (-1);			/* pixel index = icol + irow*width */
int	bgbitval=0, fgbitval=1;		/* background, foreground bitval */
int	isfirstaa = 1;			/*debugging switch signals 1st pixel*/
int	aacenterwt=centerwt, aaadjacentwt=adjacentwt, aacornerwt=cornerwt,
	totwts = centerwt + 4*(adjacentwt+cornerwt); /*pnmalias default wts*/
int	isfgalias  = fgalias,		/*(1) true to antialias fg bits */
	isfgonly   = fgonly,		/*(0) true to only antialias fg bits*/
	isbgalias  = bgalias,		/*(0) true to antialias bg bits */
	isbgonly   = bgonly;		/*(0) true to only antialias bg bits*/
int	gridnum=(-1), aagridnum(),	/* grid# for 3x3 grid at irow,icol */
	patternum=(-1), aapatternnum();	/*pattern#, 1-51, for input gridnum*/
int	aapatterns();			/* to antialias special patterns */
/* ---
 * pattern number data
 * ------------------- */
/* --- number of adjacent fg pixels set in pattern --- */
static	int nadjacents[] = { -1,	/* #adjacent fg pixels for pattern */
   0,  4,  0,  1,  4,  3,  1,  0,  1,  0,	/*  1-10 */
   2,  2,  3,  4,  3,  4,  2,  2,  1,  2,	/* 11-20 */
   1,  2,  1,  2,  0,  1,  3,  2,  3,  2,	/* 21-30 */
   3,  2,  3,  2,  4,  3,  1,  2,  2,  2,	/* 31-40 */
   2,  1,  2,  2,  3,  0,  3,  2,  2,  1,  4,	/* 41-51 */
   -1 } ; /* --- end-of-nadjacents[] --- */
/* --- number of corner fg pixels set in pattern --- */
static	int ncorners[] = { -1,		/* #corner fg pixels for pattern */
   0,  4,  1,  0,  3,  4,  1,  2,  1,  2,	/*  1-10 */
   0,  0,  3,  2,  3,  2,  4,  4,  2,  1,	/* 11-20 */
   2,  1,  2,  1,  3,  2,  0,  1,  2,  3,	/* 21-30 */
   2,  3,  2,  3,  1,  2,  4,  3,  2,  2,	/* 31-40 */
   2,  3,  2,  2,  1,  4,  1,  2,  2,  3,  0,	/* 41-51 */
   -1 } ; /* --- end-of-ncorners[] --- */
/* --- 0,1,2=pattern contains horizontal bg,fg,both edge; -1=no edge --- */
static	int horzedges[] = { -1,		/* 0,1,2 = horz bg,fg,both edge */
   0,  1,  0,  0,  1,  1,  0,  0,  0, -1,	/*  1-10 */
   0, -1,  1,  1,  1, -1,  1, -1,  2,  0,	/* 11-20 */
  -1, -1, -1,  0, -1, -1, -1, -1,  2,  1,	/* 21-30 */
  -1, -1, -1,  1, -1, -1, -1, -1,  2, -1,	/* 31-40 */
  -1,  1,  1, -1, -1, -1,  0,  0, -1, -1, -1,	/* 41-51 */
   -1 } ; /* --- end-of-horzedges[] --- */
/* --- 0,1,2=pattern contains vertical bg,fg,both edge; -1=no edge --- */
static	int vertedges[] = { -1,		/* 0,1,2 = vert bg,fg,both edge */
   0,  1,  0,  0,  1,  1,  0, -1, -1, -1,	/*  1-10 */
   0,  0,  1, -1, -1, -1,  1,  1, -1, -1,	/* 11-20 */
  -1,  0,  0,  0, -1, -1,  0, -1, -1, -1,	/* 21-30 */
  -1,  1,  1,  1, -1, -1,  1, -1, -1, -1,	/* 31-40 */
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,	/* 41-51 */
   -1 } ; /* --- end-of-vertedges[] --- */
/* --- 0,1,2=pattern contains diagonal bg,fg,both edge; -1=no edge --- */
static	int diagedges[] = { -1,		/* 0,1,2 = diag bg,fg,both edge */
   0,  1,  0,  0,  1,  1,  0,  0,  0,  0,	/*  1-10 */
   2, -1,  1,  1,  1,  1,  1, -1,  0,  2,	/* 11-20 */
   0, -1,  0,  2,  0, -1,  1,  1,  1,  1,	/* 21-30 */
   1, -1,  1,  2,  1,  1, -1,  1,  2, -1,	/* 31-40 */
   1,  0, -1,  2,  1,  0,  1, -1,  1, -1,  1,	/* 41-51 */
   -1 } ; /* --- end-of-diagedges[] --- */
/* -------------------------------------------------------------------------
Calculate bytemap as 9-point weighted average over bitmap
-------------------------------------------------------------------------- */
for ( irow=0; irow<height; irow++ )
 for ( icol=0; icol<width; icol++ )
  {
  /* --- local allocations and declarations --- */
  int	bitval=0,			/* value of rp bit at irow,icol */
	isbgdiag=0, isfgdiag=0,		/*does pixel border a bg or fg edge*/
	aabyteval=0;			/* antialiased (or unchanged) value*/
  /* --- get gridnum and center bit value, init aabyteval --- */
  imap++;				/* first set imap=icol + irow*width*/
  gridnum = aagridnum(rp,irow,icol);	/*grid# coding 3x3 grid at irow,icol*/
  bitval = (gridnum&1);			/* center bit set if gridnum odd */
  aabyteval = (intbyte)(bitval==bgbitval?0:grayscale-1); /* default aa val */
  bytemap[imap] = (intbyte)(aabyteval);	/* init antialiased pixel */
  if ( gridnum<0 || gridnum>511 ) continue; /* gridnum out of bounds*/
  /* --- check if we're antialiasing this pixel --- */
  if ( (isbgonly && bitval==fgbitval)	/* only antialias background bit */
  ||   (isfgonly && bitval==bgbitval) )	/* only antialias foreground bit */
    continue;				/* leave default and do next bit */
  /* --- look up pattern number, 1-51, corresponding to input gridnum --- */
  patternum = aapatternnum(gridnum);	/* look up pattern number */
  if ( patternum<1 || patternum>51 ) continue; /* some internal error */
  /* --- special pattern number processing --- */
  if ( (aabyteval = aapatterns(rp,irow,icol,gridnum,patternum,grayscale))
  >=   0 ) {				/* special processing for pattern */
    bytemap[imap] = (intbyte)(aabyteval); /* set antialiased pixel */
    continue; }				/* and continue with next pixel */
  /* --- check for diagonal edges --- */
  isbgdiag = ( diagedges[patternum]==2 || /*current pixel borders a bg edge*/
               diagedges[patternum]==0 );
  isfgdiag = ( diagedges[patternum]==2 || /*current pixel borders a fg edge*/
               diagedges[patternum]==1 );
  /* ---check top/bot left/right edges for corners (added by j.forkosh)--- */
  if ( 1 ) {				/* true to perform test */
    int	isbghorz=0, isfghorz=0, isbgvert=0, isfgvert=0, /* horz/vert edges */
	horzedge=horzedges[patternum], vertedge=vertedges[patternum];
    isbghorz = (horzedge==2||horzedge==0); /* top or bottom edge is all bg */
    isfghorz = (horzedge==2||horzedge==1); /* top or bottom edge is all fg */
    isbgvert = (vertedge==2||vertedge==0); /* left or right edge is all bg */
    isfgvert = (vertedge==2||vertedge==1); /* left or right edge is all fg */
    if ( (isbghorz && isbgvert && (bitval==fgbitval))	/* we're at an...*/
    ||   (isfghorz && isfgvert && (bitval==bgbitval)) )	/*...inside corner */
	continue;					/* don't antialias */
    } /* --- end-of-if(1) --- */
#if 0
  /* --- check #gaps for checkerboard (added by j.forkosh) --- */
  if ( 0 ) {				/* true to perform test */
    int	ngaps=0, mingaps=1,maxgaps=2;	/* count #fg/bg flips (max=4 noop) */
    if ( nwbitval!=nnbitval ) ngaps++;	/* upper-left =? upper */
    if ( nnbitval!=nebitval ) ngaps++;	/* upper =? upper-right */
    if ( nebitval!=eebitval ) ngaps++;	/* upper-right =? right */
    if ( eebitval!=sebitval ) ngaps++;	/* right =? lower-right */
    if ( sebitval!=ssbitval ) ngaps++;	/* lower-right =? lower */
    if ( ssbitval!=swbitval ) ngaps++;	/* lower =? lower-left */
    if ( swbitval!=wwbitval ) ngaps++;	/* lower-left =? left */
    if ( wwbitval!=nwbitval ) ngaps++;	/* left =? upper-left */
    if ( ngaps > 0 ) ngaps /= 2;	/* each gap has 2 bg/fg flips */
    if ( ngaps<mingaps || ngaps>maxgaps ) continue;
    } /* --- end-of-if(1) --- */
#endif
  /* --- antialias if necessary --- */
  if ( (isbgalias && isbgdiag)		/* alias pixel surrounding bg */
  ||   (isfgalias && isfgdiag)		/* alias pixel surrounding fg */
  ||   (isbgdiag  && isfgdiag) )	/* neighboring fg and bg pixel */
    {
    int	aasumval =			/* sum wts[]*bitmap[] */
	aacenterwt*bitval +		/* apply centerwt to center pixel */
	aaadjacentwt*nadjacents[patternum] + /* similarly for adjacents */
	aacornerwt*ncorners[patternum];	/* and corners */
    double aawtval = ((double)aasumval)/((double)totwts); /* weighted val */
    aabyteval= (int)(((double)(grayscale-1))*aawtval+0.5); /*0...grayscale-1*/
    bytemap[imap] = (intbyte)(aabyteval); /* set antialiased pixel */
    if ( msglevel>=99 && msgfp!=NULL ) { fprintf(msgfp, /*diagnostic output*/
      "%s> irow,icol,imap=%d,%d,%d aawtval=%.4f aabyteval=%d",
      (isfirstaa?"aapnmlookup algorithm":"aapnm"),
      irow,icol,imap, aawtval,aabyteval);
      if ( msglevel < 100 ) fprintf(msgfp,"\n"); /* no more output */
      else fprintf(msgfp,", grid#,pattern#=%d,%d\n",gridnum,patternum);
      isfirstaa = 0; }
    } /* --- end-of-if(isedge) --- */
  } /* --- end-of-for(irow,icol) --- */
/* -------------------------------------------------------------------------
Back to caller with gray-scale anti-aliased bytemap
-------------------------------------------------------------------------- */
/*end_of_job:*/
  return ( 1 );
} /* --- end-of-function aapnmlookup() --- */


/* ==========================================================================
 * Function:	aapatterns ( rp, irow, icol, gridnum, patternum, grayscale )
 * Purpose:	For patterns requireing special processing,
 *		calculates anti-aliased value for pixel at irow,icol,
 *		whose surrounding 3x3 pixel grid is coded by gridnum
 *		(which must correspond to a pattern requiring special
 *		processing).
 * --------------------------------------------------------------------------
 * Arguments:	rp (I)		raster *  to raster whose bitmap
 *				is to be anti-aliased
 *		irow (I)	int containing row, 0...height-1,
 *				of pixel to be antialiased
 *		icol (I)	int containing col, 0...width-1,
 *				of pixel to be antialiased
 *		gridnum (I)	int containing 0...511 corresponding to
 *				3x3 pixel grid surrounding irow,icol
 *		patternum (I)	int containing 1...51 pattern# of
 *				the 3x3 grid surrounding irow,icol
 *		grayscale (I)	int containing number of grayscales
 *				to be calculated, 0...grayscale-1
 *				(should typically be given as 256)
 * --------------------------------------------------------------------------
 * Returns:	( int )		0...grayscale-1 for success,
 *				-1 = error, or no special processing required
 * --------------------------------------------------------------------------
 * Notes:    o
 * ======================================================================= */
/* --- entry point --- */
int	aapatterns (raster *rp, int irow, int icol,
	int gridnum, int patternum, int grayscale)
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
int	aaval = (-1);			/* antialiased value returned */
int	iscenter = (gridnum&1);		/* true if center pixel set/black */
int	aapatternnum(),			/* if patternum not supplied */
	aapattern1124(),		/* routine for patterns #11,24 */
	aapattern19(),			/* special routine for pattern #19 */
	aapattern20(),			/* special routine for pattern #20 */
	aapattern39();			/* special routine for pattern #39 */
/* -------------------------------------------------------------------------
special pattern number processing
-------------------------------------------------------------------------- */
if ( 1 ) {
  if ( patternum < 1 )			/* pattern# not supplied by caller */
    patternum = aapatternnum(gridnum);	/* so look it up ourselves */
  switch ( patternum ) {
    default: break;			/* no special processing */
    case 11:
    case 24: aaval = aapattern1124(rp,irow,icol,gridnum,grayscale); break;
    case 19: aaval = aapattern19(rp,irow,icol,gridnum,grayscale); break;
    case 20: aaval = aapattern20(rp,irow,icol,gridnum,grayscale); break;
    case 39: aaval = aapattern39(rp,irow,icol,gridnum,grayscale); break;
    /* case 24: if ( (gridnum&1) == 0 ) aaval=0; break; */
    case 29: aaval = (iscenter?grayscale-1:0); break; /* no antialiasing */
    } /* --- end-of-switch(patternum) --- */
  } /* --- end-of-if() --- */
return ( aaval );			/* return antialiased val to caller*/
} /* --- end-of-function aapatterns() --- */


/* ==========================================================================
 * Function:	aapattern1124 ( rp, irow, icol, gridnum, grayscale )
 * Purpose:	calculates anti-aliased value for pixel at irow,icol,
 *		whose surrounding 3x3 pixel grid is coded by gridnum
 *		(which must correspond to pattern #11 or #24).
 * --------------------------------------------------------------------------
 * Arguments:	rp (I)		raster *  to raster whose bitmap
 *				is to be anti-aliased
 *		irow (I)	int containing row, 0...height-1,
 *				of pixel to be antialiased
 *		icol (I)	int containing col, 0...width-1,
 *				of pixel to be antialiased
 *		gridnum (I)	int containing 0...511 corresponding to
 *				3x3 pixel grid surrounding irow,icol
 *		grayscale (I)	int containing number of grayscales
 *				to be calculated, 0...grayscale-1
 *				(should typically be given as 256)
 * --------------------------------------------------------------------------
 * Returns:	( int )		0...grayscale-1 for success, -1=any error
 * --------------------------------------------------------------------------
 * Notes:    o	Handles the eight gridnum's
 *		(gridnum/2 shown to eliminate irrelevant low-order bit)
 *		  ---        ---         -*-          -*-
 *		  --* = 10   *-- = 18    --* = 72     *-- = 80  (pattern$11)
 *		  -*-        -*-         ---          ---
 *
 *		  ---        ---         -**          **-
 *		  --* = 11   *-- = 22    --* = 104    *-- = 208 (pattern$24)
 *		  -**        **-         ---          ---
 *	     o	For black * center pixel, using grid#10 as an example,
 *		pixel stays ---      antialiased  ---*
 *		black if    -***     if part of	  -**
 *		part of a   -*-      a diagonal	  -*- 
 *		corner, eg,  *       line, eg,	  *
 * ======================================================================= */
/* --- entry point --- */
int	aapattern1124 (raster *rp, int irow, int icol,
	int gridnum, int grayscale)
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
int	aaval = (-1);			/* antialiased value returned */
int	iscenter = gridnum&1;		/* true if pixel at irow,icol black*/
int	patternum = 24;			/* init for pattern#24 default */
pixbyte	*bitmap = rp->pixmap;		/* local rp->pixmap ptr */
int	width=rp->width, height=rp->height; /* width, height of raster */
int	jrow=irow, jcol=icol;		/* corner or diagonal row,col */
int	vertcornval=0, horzcornval=0,	/* vertical, horizontal corner bits*/
	topdiagval=0,  botdiagval=0,	/* upper,lower diagonal pixel bits */
	cornval=0, diagval=0;		/* vert+horzcorn, top+botdiag */
int	hdirection=99, vdirection=99,	/* horz,vert corner direction */
	hturn=99,vturn=99, aafollowline(); /* follow corner till turns */
/* -------------------------------------------------------------------------
Check corner and diagonal pixels
-------------------------------------------------------------------------- */
if ( 0 ) goto end_of_job;		/* true to turn off pattern1124 */
switch ( gridnum/2 ) {			/* check pattern#11,24 corner, diag*/
  default: goto end_of_job;		/* not a pattern#11,24 gridnum */
  case 10: patternum=11; case 11:
    hdirection = 2;  vdirection = -1;	/* directions to follow corner */
    if ( (jrow=irow+2) < height ) {	/* vert corner below center pixel */
      vertcornval = getlongbit(bitmap,(icol+jrow*width));
      if ( (icol-1) >= 0 )		/* lower diag left of center */
        botdiagval = getlongbit(bitmap,((icol-1)+jrow*width)); }
    if ( (jcol=icol+2) < width ) {	/* horz corner right of center */
      horzcornval = getlongbit(bitmap,(jcol+irow*width));
      if ( (irow-1) >= 0 )		/* upper diag above center */
        topdiagval = getlongbit(bitmap,(jcol+(irow-1)*width)); }
    break;
  case 18: patternum=11; case 22:
    hdirection = -2;  vdirection = -1;	/* directions to follow corner */
    if ( (jrow=irow+2) < height ) {	/* vert corner below center pixel */
      vertcornval = getlongbit(bitmap,(icol+jrow*width));
      if ( (icol+1) < width )		/* lower diag right of center */
        botdiagval = getlongbit(bitmap,((icol+1)+jrow*width)); }
    if ( (jcol=icol-2) >= 0 ) {		/* horz corner left of center */
      horzcornval = getlongbit(bitmap,(jcol+irow*width));
      if ( (irow-1) >= 0 )		/* upper diag above center */
        topdiagval = getlongbit(bitmap,(jcol+(irow-1)*width)); }
    break;
  case 72: patternum=11; case 104:
    hdirection = 2;  vdirection = 1;	/* directions to follow corner */
    if ( (jrow=irow-2) >= 0 ) {		/* vert corner above center pixel */
      vertcornval = getlongbit(bitmap,(icol+jrow*width));
      if ( (icol-1) >= 0 )		/* upper diag left of center */
        topdiagval = getlongbit(bitmap,((icol-1)+jrow*width)); }
    if ( (jcol=icol+2) < width ) {	/* horz corner right of center */
      horzcornval = getlongbit(bitmap,(jcol+irow*width));
      if ( (irow+1) < height )		/* lower diag below center */
        botdiagval = getlongbit(bitmap,(jcol+(irow+1)*width)); }
    break;
  case 80: patternum=11; case 208:
    hdirection = -2;  vdirection = 1;	/* directions to follow corner */
    if ( (jrow=irow-2) >= 0 ) {		/* vert corner above center pixel */
      vertcornval = getlongbit(bitmap,(icol+jrow*width));
      if ( (icol+1) < width )		/* upper diag right of center */
        topdiagval = getlongbit(bitmap,((icol+1)+jrow*width)); }
    if ( (jcol=icol-2) >= 0 ) {		/* horz corner left of center */
      horzcornval = getlongbit(bitmap,(jcol+irow*width));
      if ( (irow+1) < height )		/* lower diag below center */
        botdiagval = getlongbit(bitmap,(jcol+(irow+1)*width)); }
    break;
  } /* --- end-of-switch(gridnum/2) --- */
cornval = vertcornval+horzcornval;	/* 0=no corner bits, 1, 2=both */
diagval = topdiagval+botdiagval;	/* 0=no diag bits, 1, 2=both */
/* -------------------------------------------------------------------------
Handle white center
-------------------------------------------------------------------------- */
if ( 1 && !iscenter ) { aaval = (patternum==11?51:64);  goto end_of_job; }
/* -------------------------------------------------------------------------
Handle black center
-------------------------------------------------------------------------- */
if ( diagval > 1 ) aaval = ( patternum==24? 255:191 );
else {
  hturn = aafollowline(rp,irow,icol,hdirection);
  vturn = aafollowline(rp,irow,icol,vdirection);
  if ( vturn*hdirection < 0  && hturn*vdirection < 0 )
       aaval = ( patternum==24? 255:191 );
  else aaval = grayscale-1; }		/* actual corner */
/* -------------------------------------------------------------------------
Back to caller with grayscale antialiased value for pixel at irow,icol
-------------------------------------------------------------------------- */
end_of_job:
  if ( aaval >= 0 )			/* have antialiasing result */
   if ( msglevel>=99 && msgfp!=NULL ) fprintf(msgfp, /* diagnostic output */
    "aapattern1124> irow,icol,grid#/2=%d,%d,%d, top,botdiag=%d,%d, "
    "vert,horzcorn=%d,%d, v,hdir=%d,%d, v,hturn=%d,%d, aaval=%d\n",
    irow,icol,gridnum/2, topdiagval,botdiagval, vertcornval,horzcornval,
    vdirection,hdirection, vturn,hturn, aaval);
  return ( aaval );			/* back with antialiased value */
} /* --- end-of-function aapattern1124() --- */


/* ==========================================================================
 * Function:	aapattern19 ( rp, irow, icol, gridnum, grayscale )
 * Purpose:	calculates anti-aliased value for pixel at irow,icol,
 *		whose surrounding 3x3 pixel grid is coded by gridnum
 *		(which must correspond to pattern #19).
 * --------------------------------------------------------------------------
 * Arguments:	rp (I)		raster *  to raster whose bitmap
 *				is to be anti-aliased
 *		irow (I)	int containing row, 0...height-1,
 *				of pixel to be antialiased
 *		icol (I)	int containing col, 0...width-1,
 *				of pixel to be antialiased
 *		gridnum (I)	int containing 0...511 corresponding to
 *				3x3 pixel grid surrounding irow,icol
 *		grayscale (I)	int containing number of grayscales
 *				to be calculated, 0...grayscale-1
 *				(should typically be given as 256)
 * --------------------------------------------------------------------------
 * Returns:	( int )		0...grayscale-1 for success, -1=any error
 * --------------------------------------------------------------------------
 * Notes:    o	Handles the four gridnum's
 *		(gridnum/2 shown to eliminate irrelevant low-order bit)
 *		  ---        --*         *--          ***
 *		  --- = 7    --* = 41    *-- = 148    --- = 224
 *		  ***        --*         *--          ---
 * ======================================================================= */
/* --- entry point --- */
int	aapattern19 (raster *rp, int irow, int icol,
	int gridnum, int grayscale)
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
int	aaval = (-1);			/* antialiased value returned */
int	iscenter = gridnum&1;		/* true if pixel at irow,icol black*/
int	orientation = 1,		/* 1=vertical, 2=horizontal */
	jrow=irow, jcol=icol;		/* middle pixel of *** line */
int	turn1=0,turn2=0, aafollowline(); /* follow *** line till it turns */
/* -------------------------------------------------------------------------
Initialization and calculation of antialiased value
-------------------------------------------------------------------------- */
/* --- check input -- */
if ( iscenter ) goto end_of_job;	/* we only antialias white pixels */
/* --- set params --- */
switch ( gridnum/2 ) {			/* check pattern#19 orientation */
  default: goto end_of_job;		/* not a pattern#19 gridnum */
  case 7:   orientation=2; jrow++; break;
  case 41:  orientation=1; jcol++; break;
  case 148: orientation=1; jcol--; break;
  case 224: orientation=2; jrow--; break;
  } /* --- end-of-switch(gridnum/2) --- */
/* --- get turns in both directions --- */
if ( (turn1 = aafollowline(rp,jrow,jcol,orientation)) == 0 ) goto end_of_job;
if ( (turn2 = aafollowline(rp,jrow,jcol,-orientation)) == 0) goto end_of_job;
if ( turn1*turn2 >= 0 ) goto end_of_job; /* both turns in same direction */
/* --- weight pixel --- */
aaval = grayscale / ( 3 + min2(abs(turn1),abs(turn2)) );
/* -------------------------------------------------------------------------
Back to caller with grayscale antialiased value for pixel at irow,icol
-------------------------------------------------------------------------- */
end_of_job:
  if ( aaval >= 0 )			/* have antialiasing result */
   if ( msglevel>=99 && msgfp!=NULL ) fprintf(msgfp, /* diagnostic output */
    "aapattern19> irow,icol,grid#/2=%d,%d,%d, turn+%d,%d=%d,%d, aaval=%d\n",
    irow,icol,gridnum/2, orientation,-orientation,turn1,turn2, aaval);
  return ( aaval );			/* back with antialiased value */
} /* --- end-of-function aapattern19() --- */


/* ==========================================================================
 * Function:	aapattern20 ( rp, irow, icol, gridnum, grayscale )
 * Purpose:	calculates anti-aliased value for pixel at irow,icol,
 *		whose surrounding 3x3 pixel grid is coded by gridnum
 *		(which must correspond to pattern #20).
 * --------------------------------------------------------------------------
 * Arguments:	rp (I)		raster *  to raster whose bitmap
 *				is to be anti-aliased
 *		irow (I)	int containing row, 0...height-1,
 *				of pixel to be antialiased
 *		icol (I)	int containing col, 0...width-1,
 *				of pixel to be antialiased
 *		gridnum (I)	int containing 0...511 corresponding to
 *				3x3 pixel grid surrounding irow,icol
 *		grayscale (I)	int containing number of grayscales
 *				to be calculated, 0...grayscale-1
 *				(should typically be given as 256)
 * --------------------------------------------------------------------------
 * Returns:	( int )		0...grayscale-1 for success, -1=any error
 * --------------------------------------------------------------------------
 * Notes:    o	Handles the eight gridnum's
 *		(gridnum/2 shown to eliminate irrelevant low-order bit)
 *		  ---        ---         --*          -*-      
 *		  --* = 14   *-- = 19    --* = 42     --* = 73
 *		  **-        -**         -*-          --*     
 *
 *		  -*-        -**         *--          **-      
 *		  *-- = 84   *-- = 112   *-- = 146    --* = 200
 *		  *--        ---         -*-          ---     
 * ======================================================================= */
/* --- entry point --- */
int	aapattern20 (raster *rp, int irow, int icol,
	int gridnum, int grayscale)
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
int	aaval = (-1);			/* antialiased value returned */
int	iscenter = gridnum&1;		/* true if pixel at irow,icol black*/
int	direction = 1,			/* direction to follow ** line */
	jrow1=irow, jcol1=icol,		/* coords of * */
	jrow2=irow, jcol2=icol;		/* coords of adjacent ** pixel */
int	turn1=0,turn2=0, aafollowline(); /* follow *,** lines till turns */
/* -------------------------------------------------------------------------
Initialization and calculation of antialiased value
-------------------------------------------------------------------------- */
/* --- check input -- */
if ( 1 ) goto end_of_job;		/* don't want this one */
if ( iscenter ) goto end_of_job;	/* we only antialias white pixels */
/* --- set params --- */
switch ( gridnum/2 ) {			/* check pattern#20 orientation */
  default: goto end_of_job;		/* not a pattern#20 gridnum */
  case 14:  direction=-2; jcol1++; jrow2++; break;
  case 19:  direction=2;  jcol1--; jrow2++; break;
  case 42:  direction=1;  jrow1++; jcol2++; break;
  case 73:  direction=-1; jrow1--; jcol2++; break;
  case 84:  direction=-1; jrow1--; jcol2--; break;
  case 112: direction=2;  jcol1--; jrow2--; break;
  case 146: direction=1;  jrow1++; jcol2--; break;
  case 200: direction=-2; jcol1++; jrow2--; break;
  } /* --- end-of-switch(gridnum/2) --- */
/* --- get turns in both directions --- */
if ( (turn1=aafollowline(rp,jrow1,jcol1,-direction)) == 0 ) goto end_of_job;
if ( (turn2=aafollowline(rp,jrow2,jcol2,direction))  == 0 ) goto end_of_job;
if ( turn1*turn2 >= 0 ) goto end_of_job; /* both turns in same direction */
/* --- weight pixel --- */
aaval = grayscale / ( 3 + min2(abs(turn1),abs(turn2)) );
/* -------------------------------------------------------------------------
Back to caller with grayscale antialiased value for pixel at irow,icol
-------------------------------------------------------------------------- */
end_of_job:
  if ( aaval >= 0 )			/* have antialiasing result */
   if ( msglevel>=99 && msgfp!=NULL ) fprintf(msgfp, /* diagnostic output */
    "aapattern20> irow,icol,grid#/2=%d,%d,%d, turn%d,%d=%d,%d, aaval=%d\n",
    irow,icol,gridnum/2, -direction,direction,turn1,turn2, aaval);
  return ( aaval );			/* back with antialiased value */
} /* --- end-of-function aapattern20() --- */


/* ==========================================================================
 * Function:	aapattern39 ( rp, irow, icol, gridnum, grayscale )
 * Purpose:	calculates anti-aliased value for pixel at irow,icol,
 *		whose surrounding 3x3 pixel grid is coded by gridnum
 *		(which must correspond to pattern #39).
 * --------------------------------------------------------------------------
 * Arguments:	rp (I)		raster *  to raster whose bitmap
 *				is to be anti-aliased
 *		irow (I)	int containing row, 0...height-1,
 *				of pixel to be antialiased
 *		icol (I)	int containing col, 0...width-1,
 *				of pixel to be antialiased
 *		gridnum (I)	int containing 0...511 corresponding to
 *				3x3 pixel grid surrounding irow,icol
 *		grayscale (I)	int containing number of grayscales
 *				to be calculated, 0...grayscale-1
 *				(should typically be given as 256)
 * --------------------------------------------------------------------------
 * Returns:	( int )		0...grayscale-1 for success, -1=any error
 * --------------------------------------------------------------------------
 * Notes:    o	Handles the eight gridnum's
 *		(gridnum/2 shown to eliminate irrelevant low-order bit)
 *		  ---        ---         --*          -**      
 *		  --* = 15   *-- = 23    --* = 43     --* = 105
 *		  ***        ***         -**          --*     
 *
 *		  **-        ***         *--          ***      
 *		  *-- = 212  *-- = 240   *-- = 150    --* = 232
 *		  *--        ---         **-          ---     
 * ======================================================================= */
/* --- entry point --- */
int	aapattern39 (raster *rp, int irow, int icol,
	int gridnum, int grayscale)
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
int	aaval = (-1);			/* antialiased value returned */
int	iscenter = gridnum&1;		/* true if pixel at irow,icol black*/
int	direction = 1,			/* direction to follow ** line */
	jrow1=irow, jcol1=icol,		/* coords of * */
	jrow2=irow, jcol2=icol;		/* coords of adjacent ** pixel */
int	turn1=0,turn2=0, aafollowline(); /* follow *,** lines till turns */
/* -------------------------------------------------------------------------
Initialization and calculation of antialiased value
-------------------------------------------------------------------------- */
/* --- check input -- */
if ( iscenter ) goto end_of_job;	/* we only antialias white pixels */
/* --- set params --- */
switch ( gridnum/2 ) {			/* check pattern#39 orientation */
  default: goto end_of_job;		/* not a pattern#39 gridnum */
  case 15:  direction=-2; jcol1++; jrow2++; break;
  case 23:  direction=2;  jcol1--; jrow2++; break;
  case 43:  direction=1;  jrow1++; jcol2++; break;
  case 105: direction=-1; jrow1--; jcol2++; break;
  case 212: direction=-1; jrow1--; jcol2--; break;
  case 240: direction=2;  jcol1--; jrow2--; break;
  case 150: direction=1;  jrow1++; jcol2--; break;
  case 232: direction=-2; jcol1++; jrow2--; break;
  } /* --- end-of-switch(gridnum/2) --- */
/* --- get turns directions (tunr1==1 signals inside corner) --- */
if ( (turn1=aafollowline(rp,jrow1,jcol1,-direction)) == 1 )
  { aaval=0; goto end_of_job; }
if ( 1 ) goto end_of_job;  /* stop here for now */
if ( (turn2=aafollowline(rp,jrow2,jcol2,direction))  == 0 ) goto end_of_job;
if ( turn1*turn2 >= 0 ) goto end_of_job; /* both turns in same direction */
/* --- weight pixel --- */
aaval = grayscale / ( 3 + min2(abs(turn1),abs(turn2)) );
/* -------------------------------------------------------------------------
Back to caller with grayscale antialiased value for pixel at irow,icol
-------------------------------------------------------------------------- */
end_of_job:
  if ( aaval >= 0 )			/* have antialiasing result */
   if ( msglevel>=99 && msgfp!=NULL ) fprintf(msgfp, /* diagnostic output */
    "aapattern39> irow,icol,grid#/2=%d,%d,%d, turn%d,%d=%d,%d, aaval=%d\n",
    irow,icol,gridnum/2, -direction,direction,turn1,turn2, aaval);
  return ( aaval );			/* back with antialiased value */
} /* --- end-of-function aapattern39() --- */


/* ==========================================================================
 * Function:	aafollowline ( rp, irow, icol, direction )
 * Purpose:	starting with pixel at irow,icol, moves in
 *		specified direction looking for a "turn"
 * --------------------------------------------------------------------------
 * Arguments:	rp (I)		raster *  to raster containing pixel image
 *		irow (I)	int containing row, 0...height-1,
 *				of first pixel
 *		icol (I)	int containing col, 0...width-1,
 *				of first pixel
 *		direction (I)	int containing +1 to follow line up/north
 *				(decreasing irow), -1 to follow line
 *				down/south (increasing irow), +2 to follow
 *				line right/east (increasing icol),
 *				-2 to follow line left/west (decreasing icol)
 * --------------------------------------------------------------------------
 * Returns:	( int )		#rows or #cols traversed prior to turn,
 *				or 0 if no turn detected (or for any error).
 *				Sign is + if turn direction is right/east or
 *				up/north, or is - if left/west or down/south.
 * --------------------------------------------------------------------------
 * Notes:     o	Here are some examples illustrating turn detection in
 *		+2 (right/east) direction.  Turns in other directions
 *		are detected similarly/symmetrically.  * denotes black
 *		bits (usually fg), - denotes white bits (usually bg),
 *		and ? denotes "don't care" bit (won't affect outcome).
 *		Arrow --> points to start pixel denoted by irow,icol.
 *
 *		   *???         -???	turn=0 (no turn) is returned
 *		-->*???   or -->-???	because the start pixel isn't
 *		   *???         -???	on an edge to begin with
 *
 *		   ----         **--	turn=0 returned because the
 *		-->***-   or -->***-	line ends abruptly without
 *		   ----	        ----	turning (even the second case)
 *
 *		   ---*         ---*	turn=0 returned because the
 *		-->***-   or -->****	line forms a Y or T rather
 *		   ---*	        ---*	than turning
 *
 *		   ***-	        ****	turn=+3 returned
 *		-->***-   or -->***-	(outside corner)
 *		   ----	        ----
 *
 *		   *****        ****-	turn=-4 returned
 *		-->*****  or -->****-	(inside corner)
 *		   ----*        ----*
 *
 *		   ----*        ----*	turn=+4 returned
 *		-->****-  or -->*****	(outside or inside corner)
 *		   -----        -----
 * ======================================================================= */
/* --- entry point --- */
int	aafollowline (raster *rp, int irow, int icol, int direction)
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
pixbyte	*bitmap = rp->pixmap;		/* local rp->pixmap ptr */
int	width=rp->width, height=rp->height; /* width, height of raster */
int	drow=0, dcol=0,			/* delta row,col to follow line */
	jrow=irow, jcol=icol;		/* current row,col following line */
int	bitval=1,			/* value of rp bit at irow,icol */
	fgval=1, bgval=0,		/* "fg" is whatever bitval is */
	bitminus=0, bitplus=0;		/* value of left/down, right/up bit*/
int	isline=1, isedge=0;		/*isline signals one-pixel wide line*/
int	turn = 0,			/* detected turn back to caller */
	maxturn = maxfollow;		/* don't follow more than max pixels*/
/* -------------------------------------------------------------------------
Initialization
-------------------------------------------------------------------------- */
/* --- check input --- */
if ( irow<0 || irow>=height		/* irow out-of-bounds */
||   icol<0 || icol>=width ) goto end_of_job; /* icol out-of-bounds */
/* --- adjust maxturn for magstep --- */
if ( 1 )				/* guard */
  if ( magstep > 1 && magstep <= 10 )	/* sanity check */
    maxturn *= magstep;			/* factor in magnification */
/* --- starting bit -- see if we're following a fg (usual), or bg line --- */
bitval = getlongbit(bitmap,(icol+irow*width)); /* starting pixel (bg or fg)*/
fgval = bitval;  bgval = (1-bitval);	/* define "fg" as whatever bitval is*/
/* --- set drow,dcol corresponding to desired direction --- */
switch ( direction ) {			/* determine drow,dcol for direction*/
  default: goto end_of_job;		/* unrecognized direction arg */
  case  1: drow = (-1); break;		/* follow line up/north */
  case -1: drow =   1;  break;		/* down/south */
  case  2: dcol =   1;  break;		/* right/east */
  case -2: dcol = (-1); break; }	/* left/west */
/* --- set bitminus and bitplus --- */
if ( drow == 0 ) {			/* we're following line right/left */
  if ( irow < height )			/* there's a pixel below current */
    bitminus = getlongbit(bitmap,(icol+(irow+1)*width)); /* get it */
  if ( irow > 0 )			/* there's a pixel above current */
    bitplus = getlongbit(bitmap,(icol+(irow-1)*width)); } /* get it */
if ( dcol == 0 ) {			/* we're following line up/down */
  if ( icol < width )			/* there's a pixel to the right */
    bitplus = getlongbit(bitmap,(icol+1+irow*width)); /* get it */
  if ( icol > 0 )			/* there's a pixel to the left */
    bitminus = getlongbit(bitmap,(icol-1+irow*width)); } /* get it */
/* --- check for lack of line to follow --- */
if ( bitval == bitplus			/* starting pixel same as above */
&&   bitval == bitminus )		/* and below (or right and left) */
  goto end_of_job;			/* so there's no line to follow */
/* --- set isline and isedge (already initted for isline) --- */
if ( bitval == bitplus )		/* starting pixel same as above */
  { isedge = (-1);  isline = 0; }	/* so we're at an edge below */
if (  bitval == bitminus )		/* starting pixel same as below */
  { isedge = 1;  isline = 0; }		/* so we're at an edge above */
/* -------------------------------------------------------------------------
follow line
-------------------------------------------------------------------------- */
while ( 1 ) {				/* until turn found (or max) */
  /* --- local allocations and declarations --- */
  int	dbitval=0,			/* value of bit at jrow,jcol */
	dbitminus=0, dbitplus=0;	/* value of left/down, right/up bit*/
  /* --- bump pixel count and indexes; check for max or end-of-raster --- */
  turn++;				/* bump #pixels followed */
  jrow += drow;  jcol += dcol;		/* indexes of next pixel to check */
  if ( turn > maxturn			/* already followed max #pixels */
  ||   jrow<0 || jrow>=height		/* or jrow past end-of-raster */
  ||   jcol<0 || jcol>=width )		/* or jcol past end-of-raster */
    { turn = 0;  goto end_of_job; }	/* so quit without finding a turn */
  /* --- set current bit (dbitval) --- */
  dbitval = getlongbit(bitmap,(jcol+jrow*width)); /*value of jrow,jcol bit*/
  /* --- set dbitminus and dbitplus --- */
  if ( drow == 0 ) {			/* we're following line right/left */
    if ( irow < height )		/* there's a pixel below current */
      dbitminus = getlongbit(bitmap,(jcol+(irow+1)*width)); /* get it */
    if ( irow > 0 )			/* there's a pixel above current */
      dbitplus = getlongbit(bitmap,(jcol+(irow-1)*width)); } /* get it */
  if ( dcol == 0 ) {			/* we're following line up/down */
    if ( icol < width )			/* there's a pixel to the right */
      dbitplus = getlongbit(bitmap,(icol+1+jrow*width)); /* get it */
    if ( icol > 0 )			/* there's a pixel to the left */
      dbitminus = getlongbit(bitmap,(icol-1+jrow*width)); } /* get it */
  /* --- first check for abrupt end-of-line, or for T or Y --- */
  if ( isline != 0 )			/* abrupt end or T,Y must be a line*/
    if ( ( bgval == dbitval		/* end-of-line if pixel flips to bg*/
           && bgval == dbitplus		/* and bg same as above pixel */
           && bgval == dbitminus )	/* and below (or right and left) */
    ||   ( fgval == dbitplus		/* T or Y if fg same as above pixel*/
           && fgval == dbitminus ) )	/* and below (or right and left) */
      { turn = 0;  goto end_of_job; }	/* so we're at a T or Y */
  /* --- check for turning line --- */
  if ( isline != 0 ) {			/* turning line must be a line */
    if ( fgval == dbitminus )		/* turning down */
      { turn = -turn;  goto end_of_job; } /* so return negative turn */
    else if ( fgval == dbitplus )	/* turning up */
      goto end_of_job; }		/* so return positive turn */
  /* --- check for inside corner at edge --- */
  if ( isedge != 0 ) {			/* inside corner must be a edge */
    if ( isedge < 0 && fgval == bitminus ) /* corner below */
      { turn = -turn;  goto end_of_job; } /* so return negative turn */
    if ( isedge > 0 && fgval == bitplus ) /* corner above */
      goto end_of_job; }		/* so return positive turn */
  /* --- check for abrupt end at edge --- */
  if ( isedge != 0			/* abrupt edge end must be an edge */
  &&   fgval == dbitval )		/* and line must not end */
    if ( (isedge < 0 && bgval == bitplus) /* abrupt end above */
    ||   (isedge > 0 && bgval == bitminus) ) /* or abrupt end below */
      { turn = 0;  goto end_of_job; }	/* so edge ended abruptly */
  /* --- check for outside corner at edge --- */
  if ( isedge != 0			/* outside corner must be a edge */
  &&   bgval == dbitval ) {		/* and line must end */
    if ( isedge > 0 ) turn = -turn;	/* outside turn down from edge above*/
    goto end_of_job; }
  } /* --- end-of-while(1) --- */
/* -------------------------------------------------------------------------
Back to caller with #rows or #cols traversed, and direction of detected turn
-------------------------------------------------------------------------- */
end_of_job:
  if ( msglevel>=99 && msgfp!=NULL )	/* debugging/diagnostic output */
    fprintf(msgfp,"aafollowline> irow,icol,direction=%d,%d,%d, turn=%d\n",
    irow,icol,direction,turn);
  return ( turn );
} /* --- end-of-function aafollowline() --- */


/* ==========================================================================
 * Function:	aagridnum ( rp, irow, icol )
 * Purpose:	calculates gridnum, 0-511 (see Notes below),
 *		for 3x3 grid centered at irow,icol
 * --------------------------------------------------------------------------
 * Arguments:	rp (I)		raster *  to raster containing
 *				bitmap image (to be anti-aliased)
 *		irow (I)	int containing row, 0...height-1,
 *				at center of 3x3 grid
 *		icol (I)	int containing col, 0...width-1,
 *				at center of 3x3 grid
 * --------------------------------------------------------------------------
 * Returns:	( int )		0-511 grid number, or -1=any error
 * --------------------------------------------------------------------------
 * Notes:     o	Input gridnum is a 9-bit int, 0-511, coding a 3x3 pixel grid
 *		whose bit positions (and corresponding values) in gridnum are
 *		  876     256 128  64
 *		  504  =   32   1  16
 *		  321       8   4   2
 *		Thus, for example (*=pixel set/black, -=pixel not set/white),
 *		  *--         *--	  -**         (note that 209 is the
 *		  -*- = 259   *-- = 302   -** = 209    inverse, set<-->unset,
 *		  --*         ***         ---          of 302)
 *	      o	A set pixel is considered black, an unset pixel considered
 *		white.
 * ======================================================================= */
/* --- entry point --- */
int	aagridnum (raster *rp, int irow, int icol)
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
pixbyte	*bitmap = rp->pixmap;		/* local rp->pixmap ptr */
int	width=rp->width, height=rp->height, /* width, height of raster */
	imap = icol + irow*width;	/* pixel index = icol + irow*width */
int	bitval=0,			/* value of rp bit at irow,icol */
	nnbitval=0, nebitval=0, eebitval=0, sebitval=0,	/*adjacent vals*/
	ssbitval=0, swbitval=0, wwbitval=0, nwbitval=0,	/*compass pt names*/
	gridnum = (-1);			/* grid# 0-511 for above 9 bits */
/* -------------------------------------------------------------------------
check input
-------------------------------------------------------------------------- */
if ( irow<0 || irow>=height		/* irow out-of-bounds */
||   icol<0 || icol>=width ) goto end_of_job; /* icol out-of-bounds */
/* -------------------------------------------------------------------------
get the 9 bits comprising the 3x3 grid centered at irow,icol
-------------------------------------------------------------------------- */
/* --- get center bit --- */
bitval = getlongbit(bitmap,imap);	/* value of rp input bit at imap */
/* --- get 8 surrounding bits --- */
if ( irow > 0 )				/* nn (north) bit available */
   nnbitval = getlongbit(bitmap,imap-width); /* nn bit value */
if ( irow < height-1 )			/* ss (south) bit available */
   ssbitval = getlongbit(bitmap,imap+width); /* ss bit value */
if ( icol > 0 )				/* ww (west) bit available */
 { wwbitval = getlongbit(bitmap,imap-1); /* ww bit value */
   if ( irow > 0 )			/* nw bit available */
     nwbitval = getlongbit(bitmap,imap-width-1); /* nw bit value */
   if ( irow < height-1 )		/* sw bit available */
     swbitval = getlongbit(bitmap,imap+width-1); } /* sw bit value */
if ( icol < width-1 )			/* ee (east) bit available */
 { eebitval = getlongbit(bitmap,imap+1); /* ee bit value */
   if ( irow > 0 )			/* ne bit available */
     nebitval = getlongbit(bitmap,imap-width+1); /* ne bit value */
   if ( irow < height-1 )		/* se bit available */
     sebitval = getlongbit(bitmap,imap+width+1); } /* se bit value */
/* --- set gridnum --- */
gridnum = 0;				/* clear all bits */
if (   bitval ) gridnum = 1;		/* set1bit(gridnum,0); */
if ( nwbitval ) gridnum += 256;		/* set1bit(gridnum,8); */
if ( nnbitval ) gridnum += 128;		/* set1bit(gridnum,7); */
if ( nebitval ) gridnum += 64;		/* set1bit(gridnum,6); */
if ( wwbitval ) gridnum += 32;		/* set1bit(gridnum,5); */
if ( eebitval ) gridnum += 16;		/* set1bit(gridnum,4); */
if ( swbitval ) gridnum += 8;		/* set1bit(gridnum,3); */
if ( ssbitval ) gridnum += 4;		/* set1bit(gridnum,2); */
if ( sebitval ) gridnum += 2;		/* set1bit(gridnum,1); */
/* -------------------------------------------------------------------------
Back to caller with gridnum coding 3x3 grid centered at irow,icol
-------------------------------------------------------------------------- */
end_of_job:
  return ( gridnum );
} /* --- end-of-function aagridnum() --- */


/* ==========================================================================
 * Function:	aapatternnum ( gridnum )
 * Purpose:	Looks up the pattern number 1...51
 *		corresponding to the 3x3 pixel grid coded by gridnum 0=no
 *		pixels set (white) to 511=all pixels set (black).
 * --------------------------------------------------------------------------
 * Arguments:	gridnum (I)	int containing 0-511 coding a 3x3 pixel grid
 *				(see Notes below)
 * --------------------------------------------------------------------------
 * Returns:	( int )		1 to 51, or -1=error
 * --------------------------------------------------------------------------
 * Notes:     o	Input gridnum is a 9-bit int, 0-511, coding a 3x3 pixel grid
 *		whose bit positions (and corresponding values) in gridnum are
 *		  876     256 128  64
 *		  504  =   32   1  16
 *		  321       8   4   2
 *		Thus, for example (*=pixel set/black, -=pixel not set/white),
 *		  *--         *--	  -**         (note that 209 is the
 *		  -*- = 259   *-- = 302   -** = 209    inverse, set<-->unset,
 *		  --*         ***         ---          of 302)
 *	      o	A set pixel is considered black, an unset pixel considered
 *		white.
 *	      o	Ignoring whether the center pixel is set or unset, and
 *		taking rotation, reflection and inversion (set<-->unset)
 *		symmetries into account, there are 32 unique pixel patterns.
 *		If inversions are listed separately, there are 51 patterns.
 *	      o	Here are the 51 unique patterns, with ? always denoting the
 *		undetermined center pixel.  At the upper-left corner of each
 *		pattern is the "pattern index number" assigned to it in this
 *		function. At the upper-right is the pattern's multiplicity,
 *		i.e., the number of different patterns obtained by rotations
 *		and reflection of the illustrated one.  Inverse patters are
 *		illustrated immediately beneath the original (the first three
 *		four-pixel patterns have identical inverses).
 *		-------------------------------------------------------------
 *		No pixels set:
 *		 #1 1 (in this case, 1 signifies that rotation
 *		  ---  and reflection give no different grids)
 *		  -?-
 *		  ---
 *		Inverse, all eight pixels set
 *		 #2 1 (the inverse multiplicity is always the same)
 *		  ***
 *		  *?*
 *		  ***
 *		-------------------------------------------------------------
 *		One pixel set:
 *		 #3 4  #4 4
 *		  *--   -*-
 *		  -?-   -?-
 *		  ---   ---
 *		Inverse, seven pixels set:
 *		 #5 4  #6 4
 *		  -**   *-*
 *		  *?*   *?*
 *		  ***   ***
 *		-------------------------------------------------------------
 *		Two pixels set:
 *		 #7 8  #8 4  #9 8  10 2  11 4  12 2
 *		  **-   *-*   *--   *--   -*-   -*-
 *		  -?-   -?-   -?*   -?-   -?*   -?-
 *		  ---   ---   ---   --*   ---   -*-
 *		Inverse, six pixels set:
 *		#13 8  14 4  15 8  16 2  17 4  18 2
 *		  --*   -*-   -**   -**   *-*   *-*
 *		  *?*   *?*   *?-   *?*   *?-   *?*
 *		  ***   ***   ***   **-   ***   *-*
 *		-------------------------------------------------------------
 *		Three pixels set:
 *		#19 4  20 8  21 8  22 8  23 8  24 4  25 4  26 4  27 4  28 4
 *		  ***   **-   **-   **-   **-   **-   *-*   *-*   -*-   -*-
 *		  -?-   -?*   -?-   -?-   -?-   *?-   -?-   -?-   -?*   -?*
 *		  ---   ---   --*   -*-   *--   ---   --*   -*-   -*-   *--
 *		Inverse, five pixels set:
 *		#29 4  30 8  31 8  32 8  33 8  34 4  35 4  36 4  37 4  38 4
 *		  ---   --*   --*   --*   --*   --*   -*-   -*-   *-*   *-*
 *		  *?*   *?-   *?*   *?*   *?*   -?*   *?*   *?*   *?-   *?-
 *		  ***   ***   **-   *-*   -**   ***   **-   *-*   *-*   -**
 *		-------------------------------------------------------------
 *		Four pixels set (including inverses):
 *		#39 8  40 4  41 8  42 8  43 4  44 4  45 8  46 1
 *		  ***   **-   **-   ***   ***   **-   **-   *-*
 *		  -?*   -?-   -?*   -?-   -?-   -?*   -?*   -?-
 *		  ---   -**   *--   --*   -*-   --*   -*-   *-*
 *
 *		                  #47 8  48 4  49 4  50 8  51 1
 *		                    ---   ---   --*   --*   -*-
 *		                    *?*   *?*   *?-   *?-   *?*
 *		                    **-   *-*   **-   *-*   -*-
 * ======================================================================= */
/* --- entry point --- */
int	aapatternnum ( int gridnum )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
int	pattern = (-1);			/*pattern#, 1-51, for input gridnum*/
/* ---
 * pattern number corresponding to input gridnum/2 code
 * ( gridnum/2 strips off gridnum's low bit because it's
 * the same pattern whether or not center pixel is set )
 * --- */
static int patternnum[] = {
    1, 3, 4, 7, 3, 8, 7,19, 4, 7,11,24, 9,23,20,39,  /*   0- 15 */
    4, 9,11,20, 7,23,24,39,12,22,27,47,22,48,47,29,  /*  16- 31 */
    3, 8, 9,23,10,25,21,42, 7,19,20,39,21,42,44,34,  /*  32- 47 */
    9,26,28,41,21,50,49,30,22,43,45,33,40,32,31,13,  /*  48- 63 */
    4, 9,12,22, 9,26,22,43,11,20,27,47,28,41,45,33,  /*  64- 79 */
   11,28,27,45,20,41,47,33,27,45,51,35,45,36,35,14,  /*  80- 95 */
    7,23,22,48,21,50,40,32,24,39,47,29,49,30,31,13,  /*  96-111 */
   20,41,45,36,44,38,31,15,47,33,35,14,31,15,16, 5,  /* 112-127 */
    3,10, 9,21, 8,25,23,42, 9,21,28,49,26,50,41,30,  /* 128-143 */
    7,21,20,44,19,42,39,34,22,40,45,31,43,32,33,13,  /* 144-159 */
    8,25,26,50,25,46,50,37,23,42,41,30,50,37,38,17,  /* 160-175 */
   23,50,41,38,42,37,30,17,48,32,36,15,32,18,15, 6,  /* 176-191 */
    7,21,22,40,23,50,48,32,20,44,45,31,41,38,36,15,  /* 192-207 */
   24,49,47,31,39,30,29,13,47,31,35,16,33,15,14, 5,  /* 208-223 */
   19,42,43,32,42,37,32,18,39,34,33,13,30,17,15, 6,  /* 224-239 */
   39,30,33,15,34,17,13, 6,29,13,14, 5,13, 6, 5, 2,  /* 240-255 */
   -1 } ; /* --- end-of-patternnum[] --- */
/* -------------------------------------------------------------------------
look up pattern number for gridnum
-------------------------------------------------------------------------- */
/* --- first check input --- */
if ( gridnum<0 || gridnum>511 ) goto end_of_job; /* gridnum out of bounds */
/* --- look up pattern number, 1-51, corresponding to input gridnum --- */
pattern = patternnum[gridnum/2];	/* /2 strips off gridnum's low bit */
if ( pattern<1 || pattern>51 ) pattern = (-1); /* some internal error */
end_of_job:
  return ( pattern );			/* back to caller with pattern# */
} /* --- end-of-function aapatternnum() --- */


/* ==========================================================================
 * Function:	aalookup ( gridnum )
 * Purpose:	Looks up the grayscale value 0=white to 255=black
 *		corresponding to the 3x3 pixel grid coded by gridnum 0=no
 *		pixels set (white) to 511=all pixels set (black).
 * --------------------------------------------------------------------------
 * Arguments:	gridnum (I)	int containing 0-511 coding a 3x3 pixel grid
 * --------------------------------------------------------------------------
 * Returns:	( int )		0=white to 255=black, or -1=error
 * --------------------------------------------------------------------------
 * Notes:     o	Input gridnum is a 9-bit int, 0-511, coding a 3x3 pixel grid
 *	      o	A set pixel is considered black, an unset pixel considered
 *		white.  Likewise, the returned grayscale is 255 for black,
 *		0 for white.  You'd more typically want to use 255-grayscale
 *		so that 255 is white and 0 is black.
 *	      o	The returned number is the (lowpass) antialiased grayscale
 *		for the center pixel (gridnum bit 0) of the grid.
 * ======================================================================= */
/* --- entry point --- */
int	aalookup ( int gridnum )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
int	grayscale = (-1);		/*returned grayscale, init for error*/
int	pattern = (-1), aapatternnum();	/*pattern#, 1-51, for input gridnum*/
int	iscenter = gridnum&1;		/*low-order bit set for center pixel*/
/* --- gray scales --- */
#define	WHT 0
#define	LGT 64
#define	GRY 128
#define	DRK 192
#define	BLK 255
#if 1
/* ---
 * modified aapnm() grayscales (second try)
 * --- */
/* --- grayscale for each pattern when center pixel set/black --- */
static int grayscale1[] = { -1,		/* [0] index not used */
   BLK,BLK,BLK,BLK,242,230,BLK,BLK,BLK,160,	/*  1-10 */
/* BLK,BLK,BLK,BLK,242,230,BLK,BLK,BLK,BLK, */	/*  1-10 */
   BLK,BLK,217,230,217,230,204,BLK,BLK,166,	/* 11-20 */
   BLK,BLK,BLK,BLK,BLK,BLK,178,166,204,191,	/* 21-30 */
   204,BLK,204,191,217,204,BLK,191,178,BLK,	/* 31-40 */
   178,BLK,BLK,178,191,BLK,191,BLK,178,BLK,204,	/* 41-51 */
   -1 } ; /* --- end-of-grayscale1[] --- */
/* --- grayscale for each pattern when center pixel not set/white --- */
static int grayscale0[] = { -1,		/* [0] index not used */
   WHT,WHT,WHT,WHT,WHT,WHT,WHT,WHT,WHT,WHT,	/*  1-10 */
    64,WHT,WHT,128,115,128,WHT,WHT,WHT, 64,	/* 11-20 */
 /* 51,WHT,WHT,128,115,128,WHT,WHT,WHT, 64, */	/* 11-20 */
   WHT,WHT,WHT, 64,WHT,WHT, 76, 64,102, 89,	/* 21-30 */
   102,WHT,102,WHT,115,102,WHT, 89, 76,WHT,	/* 31-40 */
    76,WHT,WHT, 76, 89,WHT, 89,WHT, 76,WHT,102,	/* 41-51 */
   -1 } ; /* --- end-of-grayscale0[] --- */
#endif
#if 0
/* ---
 * modified aapnm() grayscales (first try)
 * --- */
/* --- grayscale for each pattern when center pixel set/black --- */
static int grayscale1[] = { -1,		/* [0] index not used */
   BLK,BLK,BLK,BLK,242,230,GRY,BLK,BLK,BLK,	/*  1-10 */
/* BLK,BLK,BLK,BLK,242,230,BLK,BLK,BLK,BLK, */	/*  1-10 */
   BLK,BLK,217,230,217,230,204,BLK,BLK,166,	/* 11-20 */
   BLK,BLK,BLK,BLK,BLK,BLK,BLK,166,204,191,	/* 21-30 */
/* BLK,BLK,BLK,BLK,BLK,BLK,178,166,204,191, */	/* 21-30 */
   204,BLK,204,BLK,217,204,BLK,191,GRY,BLK,	/* 31-40 */
/* 204,BLK,204,191,217,204,BLK,191,178,BLK, */	/* 31-40 */
   178,BLK,BLK,178,191,BLK,BLK,BLK,178,BLK,204,	/* 41-51 */
/* 178,BLK,BLK,178,191,BLK,191,BLK,178,BLK,204, */ /* 41-51 */
   -1 } ; /* --- end-of-grayscale1[] --- */
/* --- grayscale for each pattern when center pixel not set/white --- */
static int grayscale0[] = { -1,		/* [0] index not used */
   WHT,WHT,WHT,WHT,WHT,WHT,WHT,WHT,WHT,WHT,	/*  1-10 */
   GRY,WHT,WHT,128,115,128,WHT,WHT,WHT,GRY,	/* 11-20 */
/*  51,WHT,WHT,128,115,128,WHT,WHT,WHT, 64, */	/* 11-20 */
   WHT,WHT,WHT,GRY,WHT,WHT, 76, 64,102, 89,	/* 21-30 */
/* WHT,WHT,WHT, 64,WHT,WHT, 76, 64,102, 89, */	/* 21-30 */
   102,WHT,102,WHT,115,102,WHT, 89,GRY,WHT,	/* 31-40 */
/* 102,WHT,102,WHT,115,102,WHT, 89, 76,WHT, */	/* 31-40 */
    76,WHT,WHT,GRY, 89,WHT, 89,WHT, 76,WHT,102,	/* 41-51 */
/*  76,WHT,WHT, 76, 89,WHT, 89,WHT, 76,WHT,102, */ /* 41-51 */
   -1 } ; /* --- end-of-grayscale0[] --- */
#endif
#if 0
/* ---
 * these grayscales _exactly_ correspond to the aapnm() algorithm
 * --- */
/* --- grayscale for each pattern when center pixel set/black --- */
static int grayscale1[] = { -1,		/* [0] index not used */
   BLK,BLK,BLK,BLK,242,230,BLK,BLK,BLK,BLK,	/*  1-10 */
   BLK,BLK,217,230,217,230,204,BLK,BLK,166,	/* 11-20 */
   BLK,BLK,BLK,BLK,BLK,BLK,178,166,204,191,	/* 21-30 */
   204,BLK,204,191,217,204,BLK,191,178,BLK,	/* 31-40 */
   178,BLK,BLK,178,191,BLK,191,BLK,178,BLK,204,	/* 41-51 */
   -1 } ; /* --- end-of-grayscale1[] --- */
/* --- grayscale for each pattern when center pixel not set/white --- */
static int grayscale0[] = { -1,		/* [0] index not used */
   WHT,WHT,WHT,WHT,WHT,WHT,WHT,WHT,WHT,WHT,	/*  1-10 */
    51,WHT,WHT,128,115,128,WHT,WHT,WHT, 64,	/* 11-20 */
   WHT,WHT,WHT, 64,WHT,WHT, 76, 64,102, 89,	/* 21-30 */
   102,WHT,102,WHT,115,102,WHT, 89, 76,WHT,	/* 31-40 */
    76,WHT,WHT, 76, 89,WHT, 89,WHT, 76,WHT,102,	/* 41-51 */
   -1 } ; /* --- end-of-grayscale0[] --- */
#endif
/* -------------------------------------------------------------------------
look up grayscale for gridnum
-------------------------------------------------------------------------- */
/* --- first check input --- */
if ( gridnum<0 || gridnum>511 ) goto end_of_job; /* gridnum out of bounds */
/* --- look up pattern number, 1-51, corresponding to input gridnum --- */
pattern = aapatternnum(gridnum);	/* look up pattern number */
if ( pattern<1 || pattern>51 ) goto end_of_job; /* some internal error */
if ( ispatternnumcount ) {		/* counts being accumulated */
  if (iscenter)	patternnumcount1[pattern] += 1;	/* bump diagnostic count */
  else		patternnumcount0[pattern] += 1; }
/* --- look up grayscale for this pattern --- */
grayscale = ( iscenter? grayscale1[pattern] : grayscale0[pattern] );
end_of_job:
  return ( grayscale );			/* back to caller with grayscale */
} /* --- end-of-function aalookup() --- */


/* ==========================================================================
 * Function:	aalowpasslookup ( rp, bytemap, grayscale )
 * Purpose:	calls aalookup() for each pixel in rp->bitmap
 *		to create anti-aliased bytemap
 * --------------------------------------------------------------------------
 * Arguments:	rp (I)		raster *  to raster whose bitmap
 *				is to be anti-aliased
 *		bytemap (O)	intbyte * to bytemap, calculated
 *				by calling aalookup() for each pixel
 *				in rp->bitmap
 *		grayscale (I)	int containing number of grayscales
 *				to be calculated, 0...grayscale-1
 *				(should typically be given as 256)
 * --------------------------------------------------------------------------
 * Returns:	( int )		1=success, 0=any error
 * --------------------------------------------------------------------------
 * Notes:    o
 * ======================================================================= */
/* --- entry point --- */
int	aalowpasslookup (raster *rp, intbyte *bytemap, int grayscale)
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
int	width=rp->width, height=rp->height, /* width, height of raster */
	icol = 0, irow = 0, imap = (-1); /* width, height, bitmap indexes */
int	bgbitval=0 /*, fgbitval=1*/;	/* background, foreground bitval */
int	bitval=0,			/* value of rp bit at irow,icol */
	aabyteval=0;			/* antialiased (or unchanged) value*/
int	gridnum=0, aagridnum(),		/* grid# for 3x3 grid at irow,icol */
	aalookup();			/* table look up  antialiased value*/
/* -------------------------------------------------------------------------
generate bytemap by table lookup for each pixel of bitmap
-------------------------------------------------------------------------- */
for ( irow=0; irow<height; irow++ )
 for ( icol=0; icol<width; icol++ )
  {
  /* --- get gridnum and center bit value, init aabyteval --- */
  gridnum = aagridnum(rp,irow,icol);	/*grid# coding 3x3 grid at irow,icol*/
  bitval = (gridnum&1);			/* center bit set if gridnum odd */
  aabyteval = (intbyte)(bitval==bgbitval?0:grayscale-1); /* default aa val */
  imap++;				/* first bump bitmap[] index */  
  bytemap[imap] = (intbyte)(aabyteval);	/* init antialiased pixel */
  /* --- look up antialiased value for this grid --- */
  aabyteval = aalookup(gridnum);	/* look up on grid# */
  if ( aabyteval>=0 && aabyteval<=255 )	/* check for success */
    bytemap[imap] = (intbyte)(aabyteval); /* init antialiased pixel */
  } /* --- end-of-for(irow,icol) --- */
ispatternnumcount = 0;			/* accumulate counts only once */
/* -------------------------------------------------------------------------
Back to caller with gray-scale anti-aliased bytemap
-------------------------------------------------------------------------- */
/*end_of_job:*/
  return ( 1 );
} /* --- end-of-function aalowpasslookup() --- */


/* ==========================================================================
 * Function:	aasupsamp ( rp, aa, sf, grayscale )
 * Purpose:	calculates a supersampled anti-aliased bytemap
 *		for rp->bitmap, with each byte 0...grayscale-1
 * --------------------------------------------------------------------------
 * Arguments:	rp (I)		raster *  to raster whose bitmap
 *				is to be anti-aliased
 *		aa (O)		address of raster * to supersampled bytemap,
 *				calculated by supersampling rp->bitmap
 *		sf (I)		int containing supersampling shrinkfactor
 *		grayscale (I)	int containing number of grayscales
 *				to be calculated, 0...grayscale-1
 *				(should typically be given as 256)
 * --------------------------------------------------------------------------
 * Returns:	( int )		1=success, 0=any error
 * --------------------------------------------------------------------------
 * Notes:     o	If the center point of the box being averaged is black,
 *		then the entire "average" is forced black (grayscale-1)
 *		regardless of the surrounding points.  This is my attempt
 *		to avoid a "washed-out" appearance of thin (one-pixel-wide)
 *		lines, which would otherwise turn from black to a gray shade.
 * ======================================================================= */
/* --- entry point --- */
int	aasupsamp (raster *rp, raster **aa, int sf, int grayscale)
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
int	status = 0;			/* 1=success, 0=failure to caller */
int	rpheight=rp->height, rpwidth=rp->width, /*bitmap raster dimensions*/
	heightrem=0, widthrem=0,	/* rp+rem is a multiple of shrinkf */
	aaheight=0,  aawidth=0,		/* supersampled dimensions */
	aapixsz=8;			/* output pixels are 8-bit bytes */
int	maxaaval=(-9999),		/* max grayscale val set in matrix */
	isrescalemax=1;			/* 1=rescale maxaaval to grayscale */
int	irp=0,jrp=0, iaa=0,jaa=0, iwt=0,jwt=0; /*indexes, i=width j=height*/
raster	*aap=NULL, *new_raster();	/* raster for supersampled image */
raster	*aaweights();			/* get weight matrix applied to rp */
static	raster *aawts = NULL;		/* aaweights() resultant matrix */
static	int prevshrink = NOVALUE,	/* shrinkfactor from previous call */
	sumwts = 0;			/* sum of weights */
static	int blackfrac = 40,		/* force black if this many pts are */
	/*grayfrac = 20,*/
	maxwt = 10,			/* max weight in weight matrix */
	minwtfrac=10, maxwtfrac=70;	/* force light pts white, dark black*/
int	type_raster(), type_bytemap();	/* debugging display routines */
int	delete_raster();		/* delete old rasters */
/* -------------------------------------------------------------------------
Initialization
-------------------------------------------------------------------------- */
/* --- check args --- */
if ( aa == NULL ) goto end_of_job;	/* no ptr for return output arg */
*aa = NULL;				/* init null ptr for error return */
if ( rp == NULL				/* no ptr to input arg */
||   sf < 1				/* invalid shrink factor */
||   grayscale < 2 ) goto end_of_job;	/* invalid grayscale */
/* --- get weight matrix (or use current one) --- */
if ( sf != prevshrink )			/* have new shrink factor */
  { if ( aawts != NULL )		/* have unneeded weight matrix */
      delete_raster(aawts);		/* so free it */
    sumwts = 0;				/* reinitialize sum of weights */
    aawts = aaweights(sf,sf);		/* get new weight matrix */
    if ( aawts != NULL )		/* got weight matrix okay*/
      for ( jwt=0; jwt<sf; jwt++ )	/* for each row */
       for ( iwt=0; iwt<sf; iwt++ )	/* and each column */
	{ int wt = (int)(getpixel(aawts,jwt,iwt)); /* weight */
	  if ( wt > maxwt )		/* don't overweight center pts */
	    { wt = maxwt;		/* scale it back */
	      setpixel(aawts,jwt,iwt,wt); } /* and replace it in matrix */
	  sumwts += wt; }		/* add weight to sum */
    prevshrink = sf; }			/* save new shrink factor */
if ( msgfp!=NULL && msglevel>=999 )
  { fprintf(msgfp,"aasupsamp> sf=%d, sumwts=%d weights=...\n", sf,sumwts);
    type_bytemap((intbyte *)aawts->pixmap,grayscale,
    aawts->width,aawts->height,msgfp); }
/* --- calculate supersampled height,width and allocate output raster */
heightrem = rpheight%sf;		/* remainder after division... */
widthrem  = rpwidth%sf;			/* ...by shrinkfactor */
aaheight  = 1+(rpheight+sf-(heightrem+1))/sf; /* ss height */
aawidth   = 1+(rpwidth+sf-(widthrem+1))/sf; /* ss width */
if ( msgfp!=NULL && msglevel>=999 )
 { fprintf(msgfp,"aasupsamp> rpwid,ht=%d,%d wd,htrem=%d,%d aawid,ht=%d,%d\n",
   rpwidth,rpheight, widthrem,heightrem, aawidth,aaheight);
   fprintf(msgfp,"aasupsamp> dump of original bitmap image...\n");
   type_raster(rp,msgfp); }		/* ascii image of rp raster */
if ( (aap = new_raster(aawidth,aaheight,aapixsz)) /* alloc output raster*/
==   NULL ) goto end_of_job;		/* quit if alloc fails */
/* -------------------------------------------------------------------------
Step through rp->bitmap, applying aawts to each "submatrix" of bitmap
-------------------------------------------------------------------------- */
for ( jaa=0,jrp=(-(heightrem+1)/2); jrp<rpheight; jrp+=sf ) /* height */
 {
 for ( iaa=0,irp=(-(widthrem+1)/2); irp<rpwidth; irp+=sf ) /* width */
  {
  int aaval=0;				/* weighted rpvals */
  int nrp=0, mrp=0;			/* #rp bits set, #within matrix */
  for ( jwt=0; jwt<sf; jwt++ )
   for ( iwt=0; iwt<sf; iwt++ )
    {
    int i=irp+iwt, j=jrp+jwt;		/* rp->pixmap point */
    int rpval = 0;			/* rp->pixmap value at i,j */
    if ( i>=0 && i<rpwidth		/* i within actual pixmap */
    &&   j>=0 && j<rpheight )		/* ditto j */
      {	mrp++;				/* count another bit within matrix */
	rpval = (int)(getpixel(rp,j,i)); } /* get actual pixel value */
    if ( rpval != 0 )
      {	nrp++;				/* count another bit set */
	aaval += (int)(getpixel(aawts,jwt,iwt)); } /* sum weighted vals */
    } /* --- end-of-for(iwt,jwt) --- */
  if ( aaval > 0 )			/*normalize and rescale non-zero val*/
    { int aafrac = (100*aaval)/sumwts;	/* weighted percent of black points */
      /*if((100*nrp)/mrp >=blackfrac)*/	/* many black interior pts */
      if( aafrac >= maxwtfrac )		/* high weight of sampledblack pts */
	aaval = grayscale-1;		/* so set supersampled pt black */
      else if( aafrac <= minwtfrac )	/* low weight of sampledblack pts */
	aaval = 0;			/* so set supersampled pt white */
      else				/* rescale calculated weight */
	aaval = ((sumwts/2 - 1) + (grayscale-1)*aaval)/sumwts; }
  maxaaval = max2(maxaaval,aaval);	/* largest aaval so far */
  if ( msgfp!=NULL && msglevel>=999 )
    fprintf(msgfp,"aasupsamp> jrp,irp=%d,%d jaa,iaa=%d,%d"
    " mrp,nrp=%d,%d aaval=%d\n",
    jrp,irp, jaa,iaa, mrp,nrp, aaval);
  if ( jaa<aaheight && iaa<aawidth )	/* bounds check */
    setpixel(aap,jaa,iaa,aaval);	/*weighted val in supersamp raster*/
  else if( msgfp!=NULL && msglevel>=9 )	/* emit error if out-of-bounds */
    fprintf(msgfp,"aasupsamp> Error: aaheight,aawidth=%d,%d jaa,iaa=%d,%d\n",
    aaheight,aawidth, jaa,iaa);
  iaa++;				/* bump aa col index */
  } /* --- end-of-for(irp) --- */
 jaa++;					/* bump aa row index */
 } /* --- end-of-for(jrp) --- */
/* --- rescale supersampled image so darkest points become black --- */
if ( isrescalemax )			/* flag set to rescale maxaaval */
  {
  double scalef = ((double)(grayscale-1))/((double)maxaaval);
  for ( jaa=0; jaa<aaheight; jaa++ )	/* height */
   for ( iaa=0; iaa<aawidth; iaa++ )	/* width */
    { int aafrac, aaval = getpixel(aap,jaa,iaa); /* un-rescaled value */
      aaval = (int)(0.5+((double)aaval)*scalef); /*multiply by scale factor*/
      aafrac = (100*aaval)/(grayscale-1); /* fraction of blackness */
      if( aafrac >= blackfrac )		/* high weight of sampledblack pts */
	aaval = grayscale-1;		/* so set supersampled pt black */
      else if( 0&&aafrac <= minwtfrac )	/* low weight of sampledblack pts */
	aaval = 0;			/* so set supersampled pt white */
      setpixel(aap,jaa,iaa,aaval); }	/* replace rescaled val in raster */
  } /* --- end-of-if(isrescalemax) --- */
*aa = aap;				/* return supersampled image*/
status = 1;				/* set successful status */
if ( msgfp!=NULL && msglevel>=999 )
  { fprintf(msgfp,"aasupsamp> anti-aliased image...\n");
    type_bytemap((intbyte *)aap->pixmap,grayscale,
    aap->width,aap->height,msgfp);  fflush(msgfp); }
/* -------------------------------------------------------------------------
Back to caller with gray-scale anti-aliased bytemap
-------------------------------------------------------------------------- */
end_of_job:
  return ( status );
} /* --- end-of-function aasupsamp() --- */


/* ==========================================================================
 * Function:	imgsupsamp ( rp, sf, grayscale )
 * Purpose:	calculates a supersampled image of rp->pixmap,
 *		shrunk by 0<(double)sf<1, with each byte 0...grayscale-1
 * --------------------------------------------------------------------------
 * Arguments:	rp (I)		raster *  to image to be super-sampled
 *		sf (I)		double containing supersampling shrinkfactor
 *				( 0.001 <= sf <= 0.999 )
 *		grayscale (I)	int containing number of grayscales
 *				to be calculated, 0...grayscale-1
 *				(typically given as 2 or 256)
 * --------------------------------------------------------------------------
 * Returns:	( raster * )	ptr to supersampled raster, NULL=any error
 * --------------------------------------------------------------------------
 * Notes:     o	If the center point of the box being averaged is black,
 *		then the entire "average" is forced black (grayscale-1)
 *		regardless of the surrounding points.  This is my attempt
 *		to avoid a "washed-out" appearance of thin (one-pixel-wide)
 *		lines, which would otherwise turn from black to a gray shade.
 * ======================================================================= */
/* --- entry point --- */
raster	*imgsupsamp (raster *rp, double sf, int grayscale)
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
raster	*ssp=NULL, *new_raster();	/* new raster for returned ss */
int	rpheight=rp->height, rpwidth=rp->width, /* rp raster dimensions */
	ssheight=0, sswidth=0,		/* supersampled dimensions */
	wtheight=0, wtwidth=0;		/* weight matrix dimensions */
int	iss=0, jss=0;			/* ss indexes, i=width j=height */
int	delete_raster();		/* delete stale sswts rasters */
int	maxwt=10, ssweights();		/* weight matrix applied to rp */
static	raster *sswts = NULL;		/* ssweights() resultant matrix */
static	double prevsf = 0.0;		/* shrinkfactor from previous call */
static	int sumwts = 0;			/* sum of weights */
int	maxssval=(-9999),		/* max grayscale set in ss */
	isrescalemax = (grayscale>4?1:0), /* 1=rescale maxssval to grayscale*/
	minwtfrac=10, maxwtfrac=70;	/* force light pts white, dark black*/
	/*grayfrac=20, blackfrac=40; */	/* force black if this many pts are */
int	blackfrac = 30;			/* supersamp density (lower=darker) */
int	sspixsz = (grayscale>2?8:1);	/* pixsz = 1,8 for returned raster */
/* -------------------------------------------------------------------------
Initialization
-------------------------------------------------------------------------- */
/* --- check args --- */
if (  rp == NULL			/* no ptr to input arg */
   || sf<0.001 || sf>0.999		/* invalid shrink factor */
   || grayscale<2 || grayscale>256	/* invalid grayscale */
   ) goto end_of_job;			/* quit if invalid request */
/* --- supersamp raster dimensions --- */
ssheight = (int)(((double)rpheight)*sf + 0.5); /* shrink height by sf */
sswidth  = (int)(((double)rpwidth)*sf  + 0.5); /* shrink width  by sf */
if ( ssheight<1 || sswidth<1 ) goto end_of_job; /* shrunk too much */
/* --- weight matrix dimensions --- */
wtheight = max2(3.0,(1.0/sf));		/* at least 3x3-point averaging */
wtheight = 1 + (wtheight/2)*2;		/* and always an odd number */
wtwidth  = wtheight;			/* same (for now) */
/* --- get weight matrix (or use current one) --- */
if ( absval(sf-prevsf) > 0.00001 ) {	/* have new sf shrink factor */
  if (sswts!=NULL) delete_raster(sswts); /* free previous weight matrix */
  sswts = NULL;				/* let ssweights() re-alloc */
  if ( (sumwts = ssweights(wtwidth,wtheight,maxwt,&sswts)) /*new wt matrix*/
  < 0 ) goto end_of_job;		/* failed to build new weights */
  prevsf = sf; }			/* save new shrink factor */
/* --- allocate output raster */
if ( (ssp = new_raster(sswidth,ssheight,sspixsz)) /* alloc output raster */
==   NULL ) goto end_of_job;		/* quit if alloc fails */
/* -------------------------------------------------------------------------
Step through each pixel of ssp, sampling corresponding submatrix of rp
-------------------------------------------------------------------------- */
for ( jss=0; jss<ssheight; jss++ )	/* rows to ssp j=height */
 for ( iss=0; iss<sswidth; iss++ ) {	/* cols to ssp i=width */
  int jrp = (int)(((double)(jss+1))/sf), /* corresponding rp row */
      irp = (int)(((double)(iss+1))/sf); /* corresponding rp col */
  int iwt=0, jwt=0,			/* sswts indexes, i=width j=height */
      iwt0=(wtwidth/2), jwt0=(wtheight/2); /* center point of sswts */
  int ssval=0;				/* weighted rpvals */
  int nrp=0, mrp=0;			/* #rp bits set, #within matrix */
  setpixel(ssp,jss,iss,0);		/* init val in supersamp raster */
  /* --- accumulate weighted value of rp pixels --- */
  for ( jwt=0; jwt<wtheight; jwt++ )
   for ( iwt=0; iwt<wtwidth; iwt++ ) {
    int i=irp+iwt-iwt0, j=jrp+jwt-jwt0;	/* rp raster point indexes */
    if ( i>=0 && i<rpwidth		/* i within rp raster bounds */
    &&   j>=0 && j<rpheight ) {		/* ditto j */
      mrp++;				/* count another bit within matrix */
      if ( (int)(getpixel(rp,j,i))	/* get actual pixel value */
      != 0 ) {				/* pixel is set/black */
        nrp++;				/* count another pixel set */
        ssval += (int)(getpixel(sswts,jwt,iwt)); } } /* sum weights */
    } /* --- end-of-for(iwt)/for(jwt) --- */
  /* --- set weighted value in ss raster --- */
  if ( ssval > 0 ) {			/*normalize and rescale non-zero val*/
    int ssfrac = (100*ssval)/sumwts;	/* weighted percent of black points */
    if( (100*nrp)/mrp >= blackfrac	/* high number of black interior pts*/
    || ssfrac >= maxwtfrac )		/* high weight of sampled black pts */
      ssval = grayscale-1;		/* so set supersampled pt black */
    else if( ssfrac <= minwtfrac )	/* low weight of sampledblack pts */
      ssval = 0;			/* so set supersampled pt white */
    else				/* rescale calculated weight */
      ssval = ((grayscale-1)*ssval + ((sumwts/2)-1))/sumwts;
    setpixel(ssp,jss,iss,ssval);	/*weighted val in supersamp raster*/
    maxssval = max2(maxssval,ssval); }	/* largest sval so far */
  } /* --- end-of-for(iss)/for(jss) --- */
/* ---
 * rescale supersampled image so darkest points become black
 * ------------------------------------------------------------ */
if ( isrescalemax ) {			/* flag set to rescale maxaaval */
  double scalef = ((double)(grayscale-1))/((double)maxssval);
  for ( jss=0; jss<ssheight; jss++ )	/* height */
   for ( iss=0; iss<sswidth; iss++ ) {	/* width */
     int ssfrac, ssval = getpixel(ssp,jss,iss); /* un-rescaled value */
     ssval = (int)(0.5+((double)ssval)*scalef); /*multiply by scale factor*/
     ssfrac = (100*ssval)/(grayscale-1); /* fraction of blackness */
     if( ssfrac >= blackfrac )		/* high weight of sampledblack pts */
       ssval = grayscale-1;		/* so set supersampled pt black */
     else if( 0&&ssfrac <= minwtfrac )	/* low weight of sampledblack pts */
       ssval = 0;			/* so set supersampled pt white */
     setpixel(ssp,jss,iss,ssval); }	/* replace rescaled val in raster */
  } /* --- end-of-if(isrescalemax) --- */
/* -------------------------------------------------------------------------
Back to caller with supersampled image
-------------------------------------------------------------------------- */
end_of_job:
  return ( ssp );
} /* --- end-of-function imgsupsamp() --- */


/* ==========================================================================
 * Function:	ssweights ( width, height, maxwt, wts )
 *		Build "canonical" supersampling weight matrix,
 *		width x height, with maximum maxwt, in raster wts
 *		(see Notes below for discussion).
 * --------------------------------------------------------------------------
 * Arguments:	width (I)	int containing width (#cols) of returned
 *				raster/matrix wts of weights
 *		height (I)	int containing height (#rows) of returned
 *				raster/matrix wts of weights
 *		maxwt (I)	int containing maximum weight allowed
 *		wts (O)		raster ** returning calculated weights
 *				(if *wts==NULL, a new_raster() of
 *				width x height is allocated, and
 *				caller must delete_raster() it when done)
 * --------------------------------------------------------------------------
 * Returns:	( int )		sum of all width*height weights,
 *				or -1 for any error
 * --------------------------------------------------------------------------
 * Notes:     o For example, given width=7, height=5, builds the matrix
 *			1 2 3  4 3 2 1
 *			2 4 6  8 6 4 2
 *			3 6 9 12 9 6 3
 *			2 4 6  8 6 4 2
 *			1 2 3  4 3 2 1
 *		If an even dimension given, the two center numbers stay
 *		the same, e.g., 123321 for the top row if width=6.
 *	      o	For an odd square n x n matrix, the sum of all n^2
 *		weights will be ((n+1)/2)^4.
 *	      o	The largest weight (in raster of bytes) is 255,
 *		so the largest square matrix is 31 x 31.  Any weight that
 *		tries to grow beyond 255 is held constant at 255.
 * ======================================================================= */
/* --- entry point --- */
int	ssweights ( int width, int height, int maxwt, raster **wts )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
int	irow=0, icol=0,			/* height, width indexes */
	weight = 0,			/*running weight, as per Notes above*/
	totwt = (-1);			/* sum of weights in wts */
int	ssalgorithm = 99;		/* maybe for later use */
raster	*new_raster();			/* if caller wants wts allocated */
/* -------------------------------------------------------------------------
Initialization
-------------------------------------------------------------------------- */
/* --- check args --- */
if ( width<1 || height<1 || maxwt<1 ) goto end_of_job; /* bad args */
maxwt = min2(maxwt,255);		/* 255 is maximum weight allowed */
if ( wts ==  NULL ) goto end_of_job;	/* no raster address supplied */
if ( *wts == NULL )			/* caller wants wts allocated */
  if ( (*wts = new_raster(width,height,8)) /* caller must delete_raster() */
  == NULL ) goto end_of_job;		/* quit if alloc failed */
/* -------------------------------------------------------------------------
Fill weight matrix, as per Notes above
-------------------------------------------------------------------------- */
totwt = 0;				/* reset sum */
for ( irow=0; irow<height; irow++ )	/* outer loop over rows */
  for ( icol=0; icol<width; icol++ ) {	/* inner loop over cols */
    int	jrow = height-irow-1,		/* backwards irow, height-1,...,0 */
	jcol =  width-icol-1;		/* backwards icol,  width-1,...,0 */
    weight = min2(irow+1,jrow+1) * min2(icol+1,jcol+1); /* set weight */
    weight = min2(maxwt,weight);	/* clamp weight */
    if ( ssalgorithm == 1 ) weight=1;	/* force equal weights */
    setpixel(*wts,irow,icol,weight);	/* store weight in raster matrix */
    totwt += weight;			/* accumulate sum of weights */
    } /* --- end-of-for(irow,icol) --- */
end_of_job:
  return ( totwt );			/* back with sum of wts or -1=error */
} /* --- end-of-function ssweights() --- */


/* ==========================================================================
 * Function:	aacolormap ( bytemap, nbytes, colors, colormap )
 * Purpose:	searches bytemap, returning a list of its discrete values
 *		in ascending order in colors[], and returning an "image"
 *		of bytemap (where values are replaced by colors[]
 *		indexes) in colormap[].
 * --------------------------------------------------------------------------
 * Arguments:	bytemap (I)	intbyte *  to bytemap containing
 *				grayscale values (usually 0=white
 *				through 255=black) for which colors[]
 *				and colormap[] will be constructed.
 *		nbytes (I)	int containing #bytes in bytemap
 *				(usually just #rows * #cols)
 *		colors (O)	intbyte *  (to be interpreted as ints)
 *				returning a list of the discrete/different
 *				values in bytemap, in ascending value order,
 *				and with gamma correction applied
 *		colormap (O)	intbyte *  returning a bytemap "image",
 *				i.e., in one-to-one pixel correspondence
 *				with bytemap, but where the values have been
 *				replaced with corresponding colors[] indexes.
 * --------------------------------------------------------------------------
 * Returns:	( int )		#colors in colors[], or 0 for any error
 * --------------------------------------------------------------------------
 * Notes:     o
 * ======================================================================= */
/* --- entry point --- */
int	aacolormap ( intbyte *bytemap, int nbytes,
			intbyte *colors, intbyte *colormap )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
int	ncolors = 0,			/* #different values in bytemap */
	igray, grayscale = 256;		/* bytemap contains intbyte's */
intbyte	*bytevalues = NULL;		/* 1's where bytemap contains value*/
int	ibyte;				/* bytemap/colormap index */
int	isscale = 0,			/* true to scale largest val to 255*/
	isgamma = 1;			/* true to apply gamma correction */
int	maxcolors = 0;			/* maximum ncolors */
/* -------------------------------------------------------------------------
Accumulate colors[] from values occurring in bytemap
-------------------------------------------------------------------------- */
/* --- initialization --- */
if ( (bytevalues = (intbyte *)malloc(grayscale)) /*alloc bytevalues*/
==   NULL ) goto end_of_job;		/* signal error if malloc() failed */
memset(bytevalues,0,grayscale);		/* zero out bytevalues */
/* --- now set 1's at offsets corresponding to values found in bytemap --- */
for ( ibyte=0; ibyte<nbytes; ibyte++ )	/* for each byte in bytemap */
  bytevalues[(int)bytemap[ibyte]] = 1;	/*use its value to index bytevalues*/
/* --- collect the 1's indexes in colors[] --- */
for ( igray=0; igray<grayscale; igray++ ) /* check all possible values */
  if ( (int)bytevalues[igray] )		/*bytemap contains igray somewheres*/
    { colors[ncolors] = (intbyte)igray;	/* so store igray in colors */
      bytevalues[igray] = (intbyte)ncolors; /* save colors[] index */
      if ( maxcolors>0 && ncolors>=maxcolors ) /* too many color indexes */
        bytevalues[igray] = (intbyte)(maxcolors-1); /*so scale back to max*/
      ncolors++; }			/* and bump #colors */
/* --- rescale colors so largest, colors[ncolors-1], is black --- */
if ( isscale )				/* only rescale if requested */
 if ( ncolors > 1 )			/* and if not a "blank" raster */
  if ( colors[ncolors-1] > 0 )		/*and at least one pixel non-white*/
   {
   /* --- multiply each colors[] by factor that scales largest to 255 --- */
   double scalefactor = ((double)(grayscale-1))/((double)colors[ncolors-1]);
   for ( igray=1; igray<ncolors; igray++ ) /* re-scale each colors[] */
    { colors[igray] = min2(grayscale-1,(int)(scalefactor*colors[igray]+0.5));
      if (igray>5) colors[igray] = min2(grayscale-1,colors[igray]+2*igray); }
   } /* --- end-of-if(isscale) --- */
/* --- apply gamma correction --- */
if ( isgamma				/* only gamma correct if requested */
&&   gammacorrection > 0.0001 )		/* and if we have gamma correction */
 if ( ncolors > 1 )			/* and if not a "blank" raster */
  if ( colors[ncolors-1] > 0 )		/*and at least one pixel non-white*/
   {
   for ( igray=1; igray<ncolors; igray++ ) { /*gamma correct each colors[]*/
    int	grayval = colors[igray],	/* original 0=white to 255=black */
	gmax = grayscale-1;		/* should be 255 */
    double dgray=((double)(gmax-grayval))/((double)gmax); /*0=black 1=white*/
    dgray = pow(dgray,(1.0/gammacorrection)); /* apply gamma correction */
    grayval = (int)( gmax*(1.0-dgray) + 0.5 ); /* convert back to grayval */
    colors[igray] = grayval; }		/* store back in colors[] */
   } /* --- end-of-if(isgamma) --- */
/* -------------------------------------------------------------------------
Construct colormap
-------------------------------------------------------------------------- */
for ( ibyte=0; ibyte<nbytes; ibyte++ )	/* for each byte in bytemap */
  colormap[ibyte] = bytevalues[(int)bytemap[ibyte]]; /*index for this value*/
/* -------------------------------------------------------------------------
back to caller with #colors, or 0 for any error
-------------------------------------------------------------------------- */
end_of_job:
  if ( bytevalues != NULL ) free(bytevalues); /* free working memory */
  if ( maxcolors>0 && ncolors>maxcolors ) /* too many color indexes */
    ncolors = maxcolors;		/* return maximum to caller */
  return ( ncolors );			/* back with #colors, or 0=error */
} /* --- end-of-function aacolormap() --- */


/* ==========================================================================
 * Function:	aaweights ( width, height )
 *		Builds "canonical" weight matrix, width x height, in a raster
 *		(see Notes below for discussion).
 * --------------------------------------------------------------------------
 * Arguments:	width (I)	int containing width (#cols) of returned
 *				raster/matrix of weights
 *		height (I)	int containing height (#rows) of returned
 *				raster/matrix of weights
 * --------------------------------------------------------------------------
 * Returns:	( raster * )	ptr to raster containing width x height
 *				weight matrix, or NULL for any error
 * --------------------------------------------------------------------------
 * Notes:     o For example, given width=7, height=5, builds the matrix
 *			1 2 3  4 3 2 1
 *			2 4 6  8 6 4 2
 *			3 6 9 12 9 6 3
 *			2 4 6  8 6 4 2
 *			1 2 3  4 3 2 1
 *		If an even dimension given, the two center numbers stay
 *		the same, e.g., 123321 for the top row if width=6.
 *	      o	For an odd square n x n matrix, the sum of all n^2
 *		weights will be ((n+1)/2)^4.
 *	      o	The largest weight (in the allocated pixsz=8 raster) is 255,
 *		so the largest square matrix is 31 x 31.  Any weight that
 *		tries to grow beyond 255 is held constant at 255.
 *	      o	A new_raster(), pixsz=8, is allocated for the caller.
 *		To avoid memory leaks, be sure to delete_raster() when done.
 * ======================================================================= */
/* --- entry point --- */
raster	*aaweights ( int width, int height )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
raster	*new_raster(), *weights=NULL;	/* raster of weights returned */
int	irow=0, icol=0,			/* height, width indexes */
	weight = 0;			/*running weight, as per Notes above*/
/* -------------------------------------------------------------------------
Initialization
-------------------------------------------------------------------------- */
/* --- allocate raster for weights --- */
if ( (weights = new_raster(width,height,8)) /* allocate 8-bit byte raster */
==  NULL ) goto end_of_job;		/* return NULL error if failed */
/* -------------------------------------------------------------------------
Fill weight matrix, as per Notes above
-------------------------------------------------------------------------- */
for ( irow=0; irow<height; irow++ )	/* outer loop over rows */
  for ( icol=0; icol<width; icol++ )	/* inner loop over cols */
    {
    int	jrow = height-irow-1,		/* backwards irow, height-1,...,0 */
	jcol =  width-icol-1;		/* backwards icol,  width-1,...,0 */
    weight = min2(irow+1,jrow+1) * min2(icol+1,jcol+1); /* weight */
    if ( aaalgorithm == 1 ) weight=1;	/* force equal weights */
    setpixel(weights,irow,icol,min2(255,weight)); /*store weight in matrix*/
    } /* --- end-of-for(irow,icol) --- */
end_of_job:
  return ( weights );			/* back with weights or NULL=error */
} /* --- end-of-function aaweights() --- */


/* ==========================================================================
 * Function:	aawtpixel ( image, ipixel, weights, rotate )
 * Purpose:	Applies matrix of weights to the pixels
 *		surrounding ipixel in image, rotated clockwise
 *		by rotate degrees (typically 0 or 30).
 * --------------------------------------------------------------------------
 * Arguments:	image (I)	raster * to bitmap (though it can be bytemap)
 *				containing image with pixels to be averaged.
 *		ipixel (I)	int containing index (irow*width+icol) of
 *				center pixel of image for weighted average.
 *		weights (I)	raster * to bytemap of relative weights
 *				(0-255), whose dimensions (usually odd width
 *				and odd height) determine the "subgrid" of
 *				image surrounding ipixel to be averaged.
 *		rotate (I)	int containing degrees clockwise rotation
 *				(typically 0 or 30), i.e., imagine weights
 *				rotated clockwise and then averaging the
 *				image pixels "underneath" it now.
 * --------------------------------------------------------------------------
 * Returns:	( int )		0-255 weighted average, or -1 for any error
 * --------------------------------------------------------------------------
 * Notes:     o	The rotation matrix used (when requested) is
 *		    / x' \     / cos(theta)  sin(theta)/a \  / x \
 *		    |    |  =  |                          |  |   |
 *                  \ y' /     \ -a*sin(theta) cos(theta) /  \ y /
 *		where a=1 (current default) is the pixel (not screen)
 *		aspect ratio width:height, and theta is rotate (converted
 *		from degrees to radians).  Then x=col,y=row are the integer
 *		pixel coords relative to the input center ipixel, and
 *		x',y' are rotated coords which aren't necessarily integer.
 *		The actual pixel used is the one nearest x',y'.
 * ======================================================================= */
/* --- entry point --- */
int	aawtpixel ( raster *image, int ipixel, raster *weights, int rotate )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
int	aaimgval = 0,			/* weighted avg returned to caller */
	totwts=0, sumwts=0;		/* total of all wts, sum wts used */
int	pixsz = image->pixsz,		/* #bits per image pixel */
	black1=1, black8=255,		/* black for 1-bit, 8-bit pixels */
	black = (pixsz==1? black1:black8), /* black value for our image */
	scalefactor = (black1+black8-black), /* only scale 1-bit images */
	iscenter = 0;			/* set true if center pixel black */
/* --- grid dimensions and indexes --- */
int	wtheight  = weights->height,	/* #rows in weight matrix */
	wtwidth   = weights->width,	/* #cols in weight matrix */
	imgheight =   image->height,	/* #rows in image */
	imgwidth  =   image->width;	/* #cols in image */
int	wtrow,  wtrow0 = wtheight/2,	/* center row index for weights */
	wtcol,  wtcol0 = wtwidth/2,	/* center col index for weights */
	imgrow, imgrow0= ipixel/imgwidth, /* center row index for ipixel */
	imgcol, imgcol0= ipixel-(imgrow0*imgwidth); /*center col for ipixel*/
/* --- rotated grid variables --- */
static	int prevrotate = 0;		/* rotate from previous call */
static	double costheta = 1.0,		/* cosine for previous rotate */
	sintheta = 0.0;			/* and sine for previous rotate */
double	a = 1.0;			/* default aspect ratio */
/* -------------------------------------------------------------------------
Initialization
-------------------------------------------------------------------------- */
/* --- refresh trig functions for rotate when it changes --- */
if ( rotate != prevrotate )		/* need new sine/cosine */
  { costheta = cos(((double)rotate)/57.29578);	/*cos of rotate in radians*/
    sintheta = sin(((double)rotate)/57.29578);	/*sin of rotate in radians*/
    prevrotate = rotate; }		/* save current rotate as prev */
/* -------------------------------------------------------------------------
Calculate aapixel as weighted average over image points around ipixel
-------------------------------------------------------------------------- */
for ( wtrow=0; wtrow<wtheight; wtrow++ )
 for ( wtcol=0; wtcol<wtwidth; wtcol++ )
  {
  /* --- allocations and declarations --- */
  int	wt = (int)getpixel(weights,wtrow,wtcol); /* weight for irow,icol */
  int	drow = wtrow - wtrow0,		/* delta row offset from center */
	dcol = wtcol - wtcol0;		/* delta col offset from center */
  int	iscenter = 0;			/* set true if center point black */
  /* --- initialization --- */
  totwts += wt;				/* sum all weights */
  /* --- rotate (if requested) --- */
  if ( rotate != 0 )			/* non-zero rotation */
    {
    /* --- apply rotation matrix to (x=dcol,y=drow) --- */
    double dx=(double)dcol, dy=(double)drow, dtemp; /* need floats */
    dtemp = dx*costheta + dy*sintheta/a; /* save new dx' */
    dy = -a*dx*sintheta + dy*costheta;	/* dy becomes new dy' */
    dx = dtemp;				/* just for notational convenience */
    /* --- replace original (drow,dcol) with nearest rotated point --- */
    drow = (int)(dy+0.5);		/* round dy for nearest row */
    dcol = (int)(dx+0.5);		/* round dx for nearest col */
    } /* --- end-of-if(rotate!=0) --- */
  /* --- select image pixel to be weighted --- */
  imgrow = imgrow0 + drow;		/*apply displacement to center row*/
  imgcol = imgcol0 + dcol;		/*apply displacement to center col*/
  /* --- if pixel in bounds, accumulate weighted average --- */
  if ( imgrow>=0 && imgrow<imgheight )	/* row is in bounds */
   if ( imgcol>=0 && imgcol<imgwidth )	/* and col is in bounds */
    {
    /* --- accumulate weighted average --- */
    int imgval = (int)getpixel(image,imgrow,imgcol); /* image value */
    aaimgval += wt*imgval;		/* weighted sum of image values */
    sumwts += wt;			/* and also sum weights used */
    /* --- check if center image pixel black --- */
    if ( drow==0 && dcol==0 )		/* this is center ipixel */
      if ( imgval==black )		/* and it's black */
	iscenter = 1;			/* so set black center flag true */
    } /* --- end-of-if(bounds checks ok) --- */
  } /* --- end-of-for(irow,icol) --- */
if ( 0 && iscenter )			/* center point is black */
  aaimgval = black8;			/* so force average black */
else					/* center point not black */
  aaimgval =				/* 0=white ... black */
      ((totwts/2 - 1) + scalefactor*aaimgval)/totwts; /* not /sumwts; */
/*end_of_job:*/
  return ( aaimgval );
} /* --- end-of-function aawtpixel() --- */


/* ==========================================================================
 * Function:	mimetexsetmsg ( newmsglevel, newmsgfp )
 * Purpose:	Sets msglevel and msgfp, usually called from
 *		an external driver (i.e., DRIVER not defined
 *		in this module).
 * --------------------------------------------------------------------------
 * Arguments:	newmsglevel (I)	int containing new msglevel
 *				(unchanged if newmsglevel<0)
 *		newmsgfp (I)	FILE * containing new msgfp
 *				(unchanged if newmsgfp=NULL)
 * --------------------------------------------------------------------------
 * Returns:	( int )		always 1
 * --------------------------------------------------------------------------
 * Notes:     o
 * ======================================================================= */
/* --- entry point --- */
int	mimetexsetmsg ( int newmsglevel, FILE *newmsgfp )
{
/* -------------------------------------------------------------------------
set msglevel and msgfp
-------------------------------------------------------------------------- */
if ( newmsglevel >= 0 ) msglevel = newmsglevel;
if ( newmsgfp != NULL ) msgfp = newmsgfp;
return ( 1 );
} /* --- end-of-function mimetexsetmsg() --- */
#endif /* PART3 */

/* ---
 * PART1
 * ------ */
#if !defined(PARTS) || defined(PART1)
#ifdef DRIVER
/* ==========================================================================
 * Function:	main() driver for mimetex.c
 * Purpose:	emits a mime xbitmap or gif image of a LaTeX math expression
 *		entered either as
 *		    (1)	html query string from a browser (most typical), or
 *		    (2)	a query string from an html <form method="get">
 *			whose <input name="formdata"> (mostly for demo), or
 *		    (3)	command-line arguments (mostly to test).
 *		If no input supplied, expression defaults to "f(x)=x^2",
 *		treated as test (input method 3).
 *		   If args entered on command-line (or if no input supplied),
 *		output is (usually) human-viewable ascii raster images on
 *		stdout rather than the usual mime xbitmaps or gif images.
 * --------------------------------------------------------------------------
 * Command-Line Arguments:
 *		When running mimeTeX from the command-line, rather than
 *		from a browser, syntax is
 *		     ./mimetex	[-d ]		dump gif to stdout
 *				[expression	expression, e.g., x^2+y^2,
 *				|-f input_file]	or read expression from file
 *				[-m msglevel]	verbosity of debugging output
 *				[-s fontsize]	default fontsize, 0-5
 *		-d   Rather than ascii debugging output, mimeTeX dumps the
 *		     actual gif (or xbitmap) to stdout, e.g.,
 *			./mimetex  -d  x^2+y^2  > expression.gif
 *		     creates a gif file containing an image of x^2+y^2
 *		-f   Reads expression from input_file, and automatically
 *		     assumes -d switch.  The input_file may contain the
 *		     expression on one line or spread out over many lines.
 *		     MimeTeX will concatanate all lines from input_file
 *		     to construct one long expression.  Blanks, tabs, and
 *		     newlines will just be ignored.
 *		-m   0-99, controls verbosity level for debugging output
 *		     (usually used only while testing code).
 *		-s   Font size, 0-5.  As usual, the font size can
 *		     also be specified in the expression by a leading
 *		     preamble terminated by $, e.g., 3$f(x)=x^2 displays
 *		     f(x)=x^2 at font size 3.  Default font size is 2.
 * --------------------------------------------------------------------------
 * Exits:	0=success, 1=some error
 * --------------------------------------------------------------------------
 * Notes:     o For an executable that emits mime xbitmaps, compile as
 *		     cc -DXBITMAP mimetex.c -lm -o mimetex.cgi
 *		or, alternatively, for an executable that emits gif images
 *		     cc -DGIF mimetex.c gifsave.c -lm -o mimetex.cgi
 *		or for gif images with anti-aliasing
 *		     cc -DGIF -DAA mimetex.c gifsave.c -lm -o mimetex.cgi
 *		See Notes at top of file for other compile-line -D options.
 *	      o	Move executable to your cgi-bin directory and either
 *		point your browser to it directly in the form
 *		     http://www.yourdomain.com/cgi-bin/mimetex.cgi?3$f(x)=x^2
 *		or put a tag in your html document of the form
 *		     <img src="../cgi-bin/mimetex.cgi?3$f(x)=x^2"
 *		       border=0 align=absmiddle>
 *		where f(x)=x^2 (or any other expression) will be displayed
 *		either as a mime xbitmap or gif image (as per -D flag).
 * ======================================================================= */

/* -------------------------------------------------------------------------
header files and other data
-------------------------------------------------------------------------- */
/* --- (additional) standard headers --- */
/* --- other data --- */
#ifdef DUMPENVIRON
 extern	char **environ;			/* environment information */
#endif

/* -------------------------------------------------------------------------
globals for gif and png callback functions
-------------------------------------------------------------------------- */
GLOBAL(raster,*bitmap_raster,NULL);	/* use 0/1 bitmap image or */
GLOBAL(intbyte,*colormap_raster,NULL);	/* anti-aliased color indexes */
GLOBAL(int,raster_width,0);		/* width of final/displayed image */
GLOBAL(int,raster_height,0);		/* height of final/displayed image */
GLOBAL(int,raster_baseline,0);		/* baseline of final/displayed image*/
/* --- anti-aliasing flags (needed by GetPixel() as well as main()) --- */
#ifdef AA				/* if anti-aliasing requested */
  #define ISAAVALUE 1			/* turn flag on */
#else
  #define ISAAVALUE 0			/* else turn flag off */
#endif
GLOBAL(int,isaa,ISAAVALUE);		/* set anti-aliasing flag */

/* -------------------------------------------------------------------------
logging data structure, and default data to be logged
-------------------------------------------------------------------------- */
/* --- logging data structure --- */
#define	logdata	struct logdata_struct	/* "typedef" for logdata_struct*/
logdata
  {
  /* -----------------------------------------------------------------------
  environment variable name, max #chars to display, min msglevel to display
  ------------------------------------------------------------------------ */
  char	*name;				/* environment variable name */
  int	maxlen;				/* max #chars to display */
  int	msglevel;			/* min msglevel to display data */
  } ; /* --- end-of-logdata_struct --- */
/* --- data logged by mimeTeX --- */
STATIC logdata mimelog[]
#ifdef INITVALS
  =
  {
  /* ------ variable ------ maxlen msglevel ----- */
    { "QUERY_STRING",         999,    4 },
    { "REMOTE_ADDR",          999,    3 },
    { "HTTP_REFERER",         999,    3 },
    { "REQUEST_URI",          999,    5 },
    { "HTTP_USER_AGENT",      999,    3 },
    { "HTTP_X_FORWARDED_FOR", 999,    3 },
    { NULL, -1, -1 }			/* trailer record */
  } /* --- end-of-mimelog[] --- */
#endif
  ;


/* --- entry point --- */
int	main ( int argc, char *argv[]
	  #ifdef DUMPENVP
	    , char *envp[]
	  #endif
	)
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
/* --- expression to be emitted --- */
static	char exprbuffer[MAXEXPRSZ+1] = "f(x)=x^2"; /* input TeX expression */
char	*expression = exprbuffer;	/* ptr to expression */
int	size = NORMALSIZE;		/* default font size */
char	*query = getenv("QUERY_STRING"); /* getenv("QUERY_STRING") result */
char	*mimeprep();			/* preprocess expression */
int	unescape_url();			/* convert %xx's to ascii chars */
int	emitcache();			/* emit cached image if it exists */
int	/*isquery = 0, (now global)*/	/* true if input from QUERY_STRING */
	isqempty = 0,			/* true if query string empty */
	isqforce = 0,			/* true to force query emulation */
	isqlogging = 0,			/* true if logging in query mode */
	isformdata = 0,			/* true if input from html form */
	isinmemory = 1,			/* true to generate image in memory*/
	isdumpimage = 0,		/* true to dump image on stdout */
	isdumpbuffer = 0;		/* true to dump to memory buffer */
char	*getdirective(), argstring[256],/*look for control \directive's, etc*/
	*pdirective = NULL;		/* ptr to char after \directive */
/* --- rasterization --- */
subraster *rasterize(), *sp=NULL;	/* rasterize expression */
raster	*border_raster(), *bp=NULL;	/* put a border around raster */
int	delete_subraster();		/* for clean-up at end-of-job */
int	type_raster(), type_bytemap(),	/* screen dump function prototypes */
	xbitmap_raster();		/* mime xbitmap output function */
/* --- http_referer --- */
char	*referer = REFERER;		/* http_referer must contain this */
char	*inputreferer = INPUTREFERER;	/*http_referer's permitted to \input*/
int	reflevels = REFLEVELS, urlncmp(); /* cmp http_referer,server_name */
int	strreplace();			/* replace SERVER_NAME in errmsg */
char	*urlprune();			/* prune referer_match */
struct	{ char *referer; int msgnum; }	/* http_referer can't contain this */
	denyreferer[] = {		/* referer table to deny access to */
	#ifdef DENYREFERER
	  #include DENYREFERER		/* e.g.,  {"",1},  for no referer */
	#endif
	{ NULL, -999 } };		/* trailer */
char	*http_referer = getenv("HTTP_REFERER"), /* referer using mimeTeX */
	*remote_addr  = getenv("REMOTE_ADDR"), /* ip address of referer */
	*http_host    = getenv("HTTP_HOST"), /* http host for mimeTeX */
	*server_name  = getenv("SERVER_NAME"), /* server hosting mimeTeX */
	*referer_match = (!isempty(http_host)?http_host: /*match http_host*/
	  (!isempty(server_name)?server_name:(NULL))); /* or server_name */
int	ishttpreferer = (isempty(http_referer)?0:1);
int	isstrstr();			/* search http_referer for referer */
int	isinvalidreferer = 0;		/* true for inavlid referer */
int	norefmaxlen = NOREFMAXLEN;	/*max query_string len if no referer*/
/* --- gif --- */
#if defined(GIF)
  int	GetPixel();			/* feed pixels to gifsave library */
  int	GIF_Create(),GIF_CompressImage(),GIF_Close(); /* prototypes for... */
  void	GIF_SetColor(),GIF_SetTransparent(); /* ...gifsave enntry points */
#endif
char	*gif_outfile = (char *)NULL,	/* gif output defaults to stdout */
	gif_buffer[MAXGIFSZ] = "\000",	/* or gif written in memory buffer */
	cachefile[256] = "\000",	/* full path and name to cache file*/
	*md5hash=NULL, *md5str();	/* md5 hash of expression */
int	maxage = 7200;			/* max-age is two hours */
int	valign = (-9999);		/*Vertical-Align:baseline-(height-1)*/
/* --- pbm/pgm (-g switch) --- */
/*int	ispbmpgm = 0; (now global)*/	/* true to write pbm/pgm file */
int	type_pbmpgm();			/* entry point, graphic format */
char	*pbm_outfile = (char *)NULL;	/* output file defaults to stdout */
/* --- anti-aliasing --- */
intbyte	*bytemap_raster = NULL,		/* anti-aliased bitmap */
	colors[256];			/* grayscale vals in bytemap */
int	aalowpass(), aapnm(),		/*lowpass filters for anti-aliasing*/
	grayscale = 256;		/* 0-255 grayscales in 8-bit bytes */
int	ncolors=2,			/* #colors (2=b&w) */
	aacolormap();			/* build colormap from bytemap */
int	ipattern;			/*patternnumcount[] index diagnostic*/
/* --- advertisement preprocessing --- */
int	advertisement(), crc16();	/*wrap expression in advertisement*/
char	*adtemplate = NULL;		/* usually use default message */
char	*host_showad = HOST_SHOWAD;	/* show ads only on this host */
/* --- messages --- */
char	logfile[256] = LOGFILE,		/*log queries if msglevel>=LOGLEVEL*/
	cachelog[256] = CACHELOG;	/* cached image log in cachepath/ */
char	*timestamp();			/* time stamp for logged messages */
char	*strdetex();			/* remove math chars from messages */
int	logger();			/* logs environ variables */
int	ismonth();			/* check argv[0] for current month */
char	*progname = (argc>0?argv[0]:"noname"); /* name program executed as */
char	*dashes =			/* separates logfile entries */
 "--------------------------------------------------------------------------";
char	*invalid_referer_msg = msgtable[invmsgnum]; /*msg to invalid referer*/
char	*invalid_referer_match = msgtable[refmsgnum]; /*referer isn't host*/
/* -------------------------------------------------------------------------
initialization
-------------------------------------------------------------------------- */
/* --- run optional system command string --- */
#ifdef SYSTEM
  system(SYSTEM);
#endif
/* --- set global variables --- */
daemonlevel++;				/* signal other funcs to reset */
msgfp = stdout;				/* for comamnd-line mode output */
isss = issupersampling;			/* set supersampling flag */
isemitcontenttype = 1;			/* true to emit mime content-type */
iscachecontenttype = 0;			/* true to cache mime content-type */
*contenttype = '\000';			/* reset content-type:, etc. cache */
isnomath = 0;				/* true to inhibit math mode */
seclevel = SECURITY;			/* overall security level */
inputseclevel = INPUTSECURITY;		/* security level for \input{} */
counterseclevel = COUNTERSECURITY;	/* security level for \counter{} */
environseclevel = ENVIRONSECURITY;	/* security level for \environ */
ninputcmds = 0;				/* reset count of \input commands */
exitstatus=0; errorstatus=ERRORSTATUS;	/* reset exit/error status */
iscaching = ISCACHING;			/* true if caching images */
if ( iscaching ) {			/* images are being cached */
  strcpy(cachepath,CACHEPATH);		/* relative path to cached files */
  if ( *cachepath == '%' ) {		/* leading % signals cache headers */
    iscachecontenttype = 1;		/* signal caching mime content-type*/
    strsqueeze(cachepath,1); } }	/* and squeeze out leading % char */
gifSize = 0;				/* signal that image not in memory */
fgred=FGRED; fggreen=FGGREEN; fgblue=FGBLUE; /* default foreground colors */
bgred=BGRED; bggreen=BGGREEN; bgblue=BGBLUE; /* default background colors */
shrinkfactor = shrinkfactors[NORMALSIZE]; /* set shrinkfactor */
for ( ipattern=1; ipattern<=51; ipattern++ )
 patternnumcount0[ipattern] = patternnumcount1[ipattern] = 0;
/* ---
 * check QUERY_STRING query for expression overriding command-line arg
 * ------------------------------------------------------------------- */
if ( query != NULL )			/* check query string from environ */
  if ( strlen(query) >= 1 ) {		/* caller gave us a query string */
    strncpy(expression,query,MAXEXPRSZ); /* so use it as expression */
    expression[MAXEXPRSZ] = '\000';	/* make sure it's null terminated */
    if ( 0 )				/*true to remove leading whitespace*/
      while ( isspace(*expression) && *expression!='\000' )
        {strsqueeze(expression,1);}	/* squeeze out white space */
    isquery = 1; }			/* and set isquery flag */
if ( !isquery ) {			/* empty query string */
  char *host = getenv("HTTP_HOST"),	/* additional getenv("") results */
  *name = getenv("SERVER_NAME"), *addr = getenv("SERVER_ADDR");
  if ( host!=NULL || name!=NULL || addr!=NULL ) { /* assume http query */
    isquery = 1;			/* set flag to signal query */
    if ( exitstatus == 0 ) exitstatus = errorstatus; /* signal error */
    strcpy(expression,			/* and give user an error message */
    "\\red\\small\\rm\\fbox{\\begin{gather}\\LaTeX~expression~not~supplied"
    "\\\\i.e.,~no~?query\\_string~given~to~mimetex.cgi\\end{gather}}"); }
  isqempty = 1;				/* signal empty query string */
  } /* --- end-of-if(!isquery) --- */
/* ---
 * process command-line input args (if not a query)
 * ------------------------------------------------ */
if ( !isquery				/* don't have an html query string */
||   ( /*isqempty &&*/ argc>1) )	/* or have command-line args */
 {
 char	*argsignal = ARGSIGNAL,		/* signals start of mimeTeX args */
	stopsignal[32] = "--";		/* default Unix end-of-args signal */
 int	iarg=0, argnum=0,		/*argv[] index for command-line args*/
	exprarg = 0,			/* argv[] index for expression */
	infilearg = 0,			/* argv[] index for infile */
	nswitches = 0,			/* number of -switches */
	isstopsignal = 0,		/* true after stopsignal found */
	isstrict = 1/*iswindows*/,	/* true for strict arg checking */
					/*nb, windows has apache "x -3" bug*/
	nargs=0, nbadargs=0,		/* number of arguments, bad ones */
	maxbadargs = (isstrict?0:1),	/*assume query if too many bad args*/
	isgoodargs = 0;			/* true to accept command-line args*/
 if ( argsignal != NULL )		/* if compiled with -DARGSIGNAL */
  while ( argc > ++iarg )		/* check each argv[] for argsignal */
    if ( !strcmp(argv[iarg],argsignal) ) /* check for exact match */
     { argnum = iarg;			/* got it, start parsing next arg */
       break; }				/* stop looking for argsignal */
 while ( argc > ++argnum )		/* check for switches and values, */
    {
    nargs++;				/* count another command-line arg */
    if ( strcmp(argv[argnum],stopsignal) == 0 ) /* found stopsignal */
      {	isstopsignal = 1;		/* so set stopsignal flag */
	continue; }			/* and get expression after it */
    if ( !isstopsignal			/* haven't seen stopsignal switch */
    &&   *argv[argnum] == '-' )		/* and have some '-' switch */
      {
      char *field = argv[argnum] + 1;	/* ptr to char(s) following - */
      char flag = tolower(*field);	/* single char following '-' */
      int  arglen = strlen(field);	/* #chars following - */
      argnum++;		/* arg following flag/switch is usually its value */
      nswitches++;			/* another switch on command line */
      if ( isstrict &&			/* if strict checking then... */
      !isthischar(flag,"g") && arglen!=1 ) /*must be single-char switch*/
	{ nbadargs++; argnum--; }	/* so ignore longer -xxx switch */
      else				/* process single-char -x switch */
       switch ( flag ) {		/* see what user wants to tell us */
	/* --- ignore uninterpreted flag --- */
	default:  nbadargs++;                              argnum--;  break;
	/* --- adjustable program parameters (not checking input) --- */
	case 'b': isdumpimage++; isdumpbuffer++;           argnum--;  break;
	case 'd': isdumpimage++;                           argnum--;  break;
	case 'e': isdumpimage++;           gif_outfile=argv[argnum];  break;
	case 'f': isdumpimage++;                   infilearg=argnum;  break;
	case 'g': ispbmpgm++;
	     if ( arglen > 1 ) pbmpgmtype = atoi(field+1); /* -g2==>type=2 */
	     if ( 1 || *argv[argnum]=='-' ) argnum--; /*next arg is -switch*/
	     else pbm_outfile = argv[argnum]; break; /*next arg is filename*/
	case 'm': if ( argnum < argc ) msglevel = atoi(argv[argnum]); break;
	case 'o': istransparent = (istransparent?0:1);     argnum--;  break;
	case 'q': isqforce = 1;                            argnum--;  break;
	case 's': if ( argnum < argc ) size = atoi(argv[argnum]);     break;
	} /* --- end-of-switch(flag) --- */
      } /* --- end-of-if(*argv[argnum]=='-') --- */
    else				/* expression if arg not a -flag */
      if ( infilearg == 0 )		/* no infile arg yet */
	{ if ( exprarg != 0 ) nbadargs++; /* 2nd expression invalid */
	  exprarg = argnum;		/* but take last expression */
	  /*infilearg = (-1);*/ }	/* and set infilearg */
      else nbadargs++;			/* infile and expression invalid */
    } /* --- end-of-while(argc>++argnum) --- */
 if ( msglevel>=999 && msgfp!=NULL )	/* display command-line info */
  { fprintf(msgfp,"argc=%d, progname=%s, #args=%d, #badargs=%d\n",
    argc,progname,nargs,nbadargs);
    fprintf(msgfp,"cachepath=\"%.50s\" pathprefix=\"%.50s\"\n",
    cachepath,pathprefix); }
 /* ---
  * decide whether command-line input overrides query_string
  * -------------------------------------------------------- */
 if ( isdumpimage > 2 ) nbadargs++;	/* duplicate/conflicting -switch */
 isgoodargs =  ( !isstrict		/*good if not doing strict checking*/
  || !isquery				/* or if no query, must use args */
  || (nbadargs<nargs && nbadargs<=maxbadargs) ); /* bad args imply query */
 /* ---
  * take expression from command-line args
  * -------------------------------------- */
 if ( isgoodargs && exprarg > 0		/* good expression on command line */
 &&   infilearg <= 0 )			/* and not given in input file */
  if ( !isquery				/* no conflict if no query_string */
  ||   nswitches > 0 )			/* explicit -switch(es) also given */
   { strncpy(expression,argv[exprarg],MAXEXPRSZ); /*expr from command-line*/
     expression[MAXEXPRSZ] = '\000';	/* make sure it's null terminated */
     isquery = 0; }			/* and not from a query_string */
 /* ---
  * or read expression from input file
  * ---------------------------------- */
 if ( isgoodargs && infilearg > 0 )	/* have a good -f arg */
  {
  FILE *infile = fopen(argv[infilearg],"r"); /* open input file for read */
  if ( infile != (FILE *)NULL )		/* opened input file successfully */
   { char instring[MAXLINESZ+1];	/* line from file */
     int  exprsz = 0;			/* total #bytes read from file */
     isquery = 0;			/* file input, not a query_string */
     *expression = '\000';		/* start expresion as empty string */
     while ( fgets(instring,MAXLINESZ,infile) != (char *)NULL ) /*till eof*/
      if ( exprsz + strlen(instring) < MAXEXPRSZ ) { /* have room for line */
	strcat(expression,instring);	/* concat line to end of expression*/
	exprsz += strlen(instring); }	/* update expression buffer length */
     fclose ( infile ); }	/*close input file after reading expression*/
  } /* --- end-of-if(infilearg>0) --- */
 /* ---
  * xlate +++'s to blanks only if query
  * ----------------------------------- */
 if ( !isquery ) isplusblank = 0;	/* don't xlate +++'s to blanks */
 /* ---
  * check if emulating query (for testing)
  * -------------------------------------- */
 if ( isqforce ) isquery = 1;		/* emulate query string processing */
 /* ---
  * check if emitting pbm/pgm graphic
  * --------------------------------- */
 if ( isgoodargs && ispbmpgm > 0 )	/* have a good -g arg */
  if ( 1 && gif_outfile != NULL )	/* had an -e switch with file */
   if ( *gif_outfile != '\000' )	/* make sure string isn't empty */
     { pbm_outfile = gif_outfile;	/* use -e switch file for pbm/pgm */
       gif_outfile = (char *)NULL;	/* reset gif output file */
       /*isdumpimage--;*/ }		/* and decrement -e count */
 /* ---
  * permit \input, etc in command-line mode
  * ------------------------------------------ */
 inputseclevel = (99999);		/* \input allowed */
 } /* --- end-of-if(!isquery) --- */
/* ---
 * check for <form> input
 * ---------------------- */
if ( isquery ) {			/* must be <form method="get"> */
 if ( !memcmp(expression,"formdata",8) ) /*must be <input name="formdata"> */
  { char *delim=strchr(expression,'=');	/* find equal following formdata */
    if ( delim != (char *)NULL )	/* found unescaped equal sign */
      {strsqueezep(expression,delim+1);} /* so shift name= out */
    while ( (delim=strchr(expression,'+')) != NULL ) /*unescaped plus sign*/
      *delim = ' ';			/* is "shorthand" for blank space */
    /*unescape_url(expression,1);*/	/* convert unescaped %xx's to chars */
    unescape_url(expression,0);		/* convert all %xx's to chars */
    unescape_url(expression,0);		/* repeat */
    if(0) msglevel = FORMLEVEL;		/* msglevel for forms */
    isformdata = 1; }			/* set flag to signal form data */
 else /* --- query, but not <form> input --- */
    unescape_url(expression,0); }	/* convert _all_ %xx's to chars */
md5hash = md5str(expression);		/* md5 hash of unedited expression */
/* ---
 * check queries for prefixes/suffixes/embedded that might cause problems
 * ---------------------------------------------------------------------- */
/* --- expression whose last char is \ --- */
if ( lastchar(expression) == '\\' )	/* last char is backslash */
  strcat(expression," ");		/* assume "\ " lost the final space*/
/* ---
 * check queries for embedded prefixes signalling special processing
 * ----------------------------------------------------------------- */
if ( isquery )				/* only check queries */
  {
  if ( getdirective(expression,"\\dumpgif",1,0,1,argstring)/*\dumpgif{level}*/
  !=   NULL ) {				/* found \dumpgif{level} */
    isdumpgif = atoi(argstring);	/* interpret arg as isdumpgif value */
    if ( isdumpgif>1 ) { *logfile = '\000'; /* default logfile for dumpgif */
      if ( !isempty(cachepath) ) strcat(logfile,cachepath);
      strcat(logfile,DUMPGIF); strcat(logfile,".txt");
      if ( msglevel < LOGLEVEL ) msglevel=LOGLEVEL; }
    } /* --- end-of-if(getdirective("\\dumpgif")!=NULL) --- */
  if ( 0 ) /* --- done by getdirective() immediately above --- */
    if ( strstr(expression,"\\dumpgif")  != NULL ) { /* debug dump gif image*/
      char *dumpgif = strstr(expression,"\\dumpgif"); /* ptr to \dumpgif */
      strsqueeze(dumpgif,strlen("\\dumpgif")); /* get rid of \dumpgif */
      isdumpgif = 1; }			/* set flag to dump gif image */
  if ( getdirective(expression,"\\msglevel",1,0,1,argstring)/*\msglevel{lev}*/
  !=   NULL ) {				/* found \msglevel{level} */
    if ( 1 || (seclevel<=9) )		/* permit msglevel specification */
      msglevel = atoi(argstring); }	/* interpret msglevel */
  if ( getdirective(expression,"\\logfile",1,0,1,argstring) /*\logfile{file}*/
  !=   NULL ) {				/* found \logfile{file} */
    if ( 1 || (seclevel<=3) )		/* permit logfile specification */
      strcpy(logfile,argstring); }	/* copy \logfile{filename} */
  if ( 0 ) { /* --- done by getdirective()'s immediately above --- */
    /* --- check for msglevel=###$ prefix --- */
    if ( !memcmp(expression,"msglevel=",9) ) { /* query has msglevel prefix */
     char *delim=strchr(expression,'$'); /* find $ delim following msglevel*/
     if ( delim != (char *)NULL )	/* check that we found delim */
      {	*delim = '\000';		/* replace delim with null */
	if ( seclevel <= 9 )		/* permit msglevel specification */
	  msglevel = atoi(expression+9); /* interpret ### in msglevel###$ */
	strsqueezep(expression,delim+1); } } /* squeeze out prefix & delim */
    /* --- next check for logfile=xxx$ prefix (must follow msglevel) --- */
    if ( !memcmp(expression,"logfile=",8) ) { /* query has logfile= prefix */
      char *delim=strchr(expression,'$'); /* find $ delim following logfile=*/
      if ( delim != (char *)NULL )	/* check that we found delim */
       { *delim = '\000';		/* replace delim with null */
	 if ( seclevel <= 3 )		/* permit logfile specification */
	   strcpy(logfile,expression+8); /* interpret xxx in logfile=xxx$ */
	 strsqueezep(expression,delim+1); } } /* squeeze out prefix & delim */
    } /* --- end-of-if(0) --- */
  } /* --- end-of-if(isquery) --- */

/* ---
 * log query (e.g., for debugging)
 * ------------------------------- */
if ( isquery )				/* only log query_string's */
 if ( (msglevel >= LOGLEVEL)		/* check if logging */
 &&   seclevel <= 5 )			/* and if logging permitted */
  if ( !isempty(logfile) ) {		/* if a logfile is given */
    if ( (msgfp=fopen(logfile,"a"))	/* open logfile for append */
    !=   NULL )				/* ignore logging if can't open */
     {
     /* --- default logging --- */
     logger(msgfp,msglevel,expression,mimelog); /* log query */
     /* --- additional debug logging (argv and environment) --- */
     if ( msglevel >= 9 )		/* log environment */
      { int i;  /*char name[999],*value;*/
	fprintf(msgfp,"Command-line arguments...\n");
	if ( argc < 1 )			/* no command-line args */
	 fprintf(msgfp,"  ...argc=%d, no argv[] variables\n",argc);
	else
	 for ( i=0; i<argc; i++ )	/* display all argv[]'s */
	  fprintf(msgfp,"  argv[%d] = \"%s\"\n",i,argv[i]);
	#ifdef DUMPENVP			/* char *envp[] available for dump */
	fprintf(msgfp,"Environment variables (using envp[])...\n");
	if ( envp == (char **)NULL )	/* envp not provided */
	 fprintf(msgfp,"  ...envp[] environment variables not available\n");
	else
	 for ( i=0; ; i++ )		/* display all envp[]'s */
	  if ( envp[i] == (char *)NULL ) break;
	  else fprintf(msgfp,"  envp[%d] = \"%s\"\n",i,envp[i]);
	#endif /* --- DUMPENVP ---*/
	#ifdef DUMPENVIRON	/* skip what should be redundant output */
	fprintf(msgfp,"Environment variables (using environ)...\n");
	if ( environ == (char **)NULL )	/* environ not provided */
	 fprintf(msgfp,"  ...extern environ variables not available\n");
	else
	 for ( i=0; ; i++ )		/*display environ[] and getenv()'s*/
	  if ( environ[i] == (char *)NULL ) break;
	  else {
	    strcpy(name,environ[i]);	/* set up name for getenv() arg */
	    if ( (value=strchr(name,'=')) != NULL ) /* = delimits name */
	      {	*value = '\000';	/* got it, so null-terminate name */
		value = getenv(name); }	/* and look up name using getenv() */
	    else strcpy(name,"NULL");	/* missing = delim in environ[i] */
	    fprintf(msgfp,"environ[%d]: \"%s\"\n\tgetenv(%s) = \"%s\"\n",
	    i,environ[i],name,(value==NULL?"NULL":value));
	    } /* --- end-of-if/else --- */
	#endif /* --- DUMPENVIRON ---*/
      } /* --- end-of-if(msglevel>=9) --- */
     /* --- close log file if no longer needed --- */
     if ( (msglevel<DBGLEVEL) && !isdumpgif ) /*logging, but not debugging*/
      {	fprintf(msgfp,"%s\n",dashes);	/* so log separator line, */
	fclose(msgfp);			/* close logfile immediately, */
	msgfp = NULL; }			/* and reset msgfp pointer */
     else
	isqlogging = 1;			/* set query logging flag */
     } /* --- end-of-if(msglevel>=LOGLEVEL) --- */
    else				/* couldn't open logfile */
     msglevel = 0; }			/* can't emit messages */
/* ---
 * prepend prefix to submitted expression
 * -------------------------------------- */
if ( 1 || isquery )			/* queries or command-line */
 if ( *exprprefix != '\000' )		/* we have a prefix string */
  { int npref = strlen(exprprefix);	/* #chars in prefix */
    memmove(expression+npref+1,expression,strlen(expression)+1);/*make room*/
    memcpy(expression,exprprefix,npref); /* copy prefix into expression */
    expression[npref] = '{';		/* followed by { */
    strcat(expression,"}"); }		/* and terminating } to balance { */
/* ---
 * preprocess expression, converting LaTeX constructs for mimeTeX,
 * and getting \password{} for referer checks
 * ------------------------------------------------------------------ */
if ( 1 && expression!=NULL ) {		/* have expression to rasterize */
  expression = mimeprep(expression);	/* preprocess expression */
  if ( *password != '\000') {		/* have \password{} from user */
    if ( strcmp(password,PASSWORD) == 0 ) /*and it matches #define'd PASSWORD*/
      ispassword = 1;			/* set flag for successful password */
    if ( 1 && !isdumpimage && !isquery	/* display password */
    &&   msglevel>=1 && msgfp!=NULL )
      fprintf(msgfp,"mimeTeX> \\password{%s} %s\n",
      password, (ispassword==1?"successful":"failed") ); }
  } /* --- end-of-if(expression!=NULL) --- */
/* ---
 * check if http_referer is allowed to use this image and to use \input{}
 * ---------------------------------------------------------------------- */
if ( isquery				/* not relevant if "interactive" */
&&   !ispassword ) {			/* nor if user supplied password */
 /* --- check -DREFERER=\"comma,separated,list\" of valid referers --- */
 if ( referer != NULL ) {		/* compiled with -DREFERER=\"...\" */
  if ( strcmp(referer,"month") != 0 )	/* but it's *only* "month" signal */
   if ( ishttpreferer )			/* or called "standalone" */
    if ( !isstrstr(http_referer,referer,0) ) { /* invalid http_referer */
      expression = invalid_referer_msg; /* so give user error message */
      isinvalidreferer = 10; } }	/* and signal invalid referer */
 else					/* compiled without -DREFERER= */
  if ( reflevels > 0 ) {		/*match referer unless -DREFLEVELS=0*/
   /* --- check topmost levels of http_referer against http_host --- */
   if ( ishttpreferer			/* have http_referer */
   &&   !isempty(referer_match) )	/* and something to match it with */
    if ( !urlncmp(http_referer,referer_match,reflevels) ) { /*match failed*/
     strcpy(exprbuffer,invalid_referer_match); /* init error message */
     strreplace(exprbuffer,"SERVER_NAME", /* and then replace SERVER_NAME */
       strdetex(urlprune(referer_match,reflevels),1),0);/*with referer_match*/
     isinvalidreferer = 20; }		/* and signal invalid referer */
   } /* --- end-of-if(reflevels>0) --- */
 } /* --- end-of-if(isquery&&!ispassword) --- */
if ( isquery ) {			/* not relevant if "interactive" */
 /* --- check -DINPUTREFERER=\"comma,separated,list\" of \input users --- */
 inputseclevel = INPUTSECURITY;		/* set default input security */
 if ( inputreferer != NULL ) {		/* compiled with -DINPUTREFERER= */
  if ( http_referer == NULL )		/* but no http_referer given */
   inputseclevel = (-1);		/* unknown user can't \input{} */
  else					/*have inputreferer and http_referer*/
   if ( !isstrstr(http_referer,inputreferer,0) ) /*http_referer can't \input*/
    inputseclevel = (-1);		/* this known user can't \input{} */
  } /* --- end-of-if(inputreferer!=NULL) --- */
 } /* --- end-of-if(isquery) --- */
/* ---
 * check if referer contains "month" signal
 * ---------------------------------------- */
if ( isquery				/* not relevant if "interactive" */
&&   !ispassword )			/* nor if user supplied password */
 if ( referer != NULL )			/* nor if compiled w/o -DREFERER= */
  if ( !isinvalidreferer )		/* nor if already invalid referer */
   if ( strstr(referer,"month") != NULL ) /* month check requested */
    if ( !ismonth(progname) )		/* not executed as mimetexJan-Dec */
     { expression = invalid_referer_msg; /* so give user error message */
       isinvalidreferer = 30; }		/* and signal invalid referer */
/* ---
 * check if http_referer is to be denied access
 * -------------------------------------------- */
if ( isquery				/* not relevant if "interactive" */
&&   !ispassword )			/* nor if user supplied password */
 if ( !isinvalidreferer )		/* nor if already invalid referer */
  { int	iref=0, msgnum=(-999),		/* denyreferer index, message# */
    whundredths = 0;			/* or wait hundredths of a sec */
    for ( iref=0; msgnum<0; iref++ ) {	/* run through denyreferer[] table */
      char *deny = denyreferer[iref].referer; /* referer to be denied */
      if ( deny == NULL ) break;	/* null signals end-of-table */
      if ( msglevel>=999 && msgfp!=NULL ) /* debugging */
	{fprintf(msgfp,"main> invalid iref=%d: deny=%s http_referer=%s\n",
	 iref,deny,(http_referer==NULL?"null":http_referer)); fflush(msgfp);}
      if ( *deny == '\000' )		/* signal to check for no referer */
	{ if ( http_referer == NULL )	/* http_referer not supplied */
	   msgnum = denyreferer[iref].msgnum; } /* so set message# */
      else				/* have referer to check for */
       if ( http_referer != NULL )	/* and have referer to be checked */
	if ( isstrstr(http_referer,deny,0) ) /* invalid http_referer */
	 msgnum = denyreferer[iref].msgnum; /* so set message# */
      } /* --- end-of-for(iref) --- */
    if ( msgnum >= 100 ) {		/* wait but don't deny */
      whundredths = 10*(msgnum-100);	/* hundredths=10*tenths */
      msgnum = (-999); }		/* reset valid referer */
    if ( msgnum >= 0 ) {		/* deny access to this referer */
      if ( msgnum > maxmsgnum ) msgnum = 0; /* keep index within bounds */
      expression = msgtable[msgnum];	/* set user error message */
      isinvalidreferer = 40; }		/* and signal invalid referer */
  } /* --- end-of-if(!isinvalidreferer) --- */
/* --- also check maximum query_string length if no http_referer given --- */
if ( isquery				/* not relevant if "interactive" */
&&   !ispassword )			/* nor if user supplied password */
 if ( !isinvalidreferer )		/* nor if already invalid referer */
  if ( !ishttpreferer ) {		/* no http_referer supplied */
   if ( strlen(expression) > norefmaxlen ) { /* query_string too long */
    if ( isempty(referer_match) )	/* no referer_match to display */
     expression = invalid_referer_msg;	/* set invalid http_referer message*/
    else {				/* error with referer_match display*/
     strcpy(exprbuffer,invalid_referer_match); /* init error message */
     strreplace(exprbuffer,"SERVER_NAME", /* and then replace SERVER_NAME */
       strdetex(urlprune(referer_match,reflevels),1),0); } /*with host_http*/
     isinvalidreferer = 50; }		/* and signal invalid referer */
   } /* --- end-of-if(!ishttpreferer) --- */
if ( isdumpgif ) {
  sprintf(argstring,"isinvalidreferer=%d\n",isinvalidreferer);
  logdump(0,argstring); }
/* ---
 * check for "advertisement"
 * ------------------------- */
/* --- check if advertisement messages only for one particular host --- */
if ( !isempty(host_showad) )		/* messages only for this referer */
 if ( !isempty(referer_match) )		/* have HTTP_HOST or SERVER_NAME */
   if ( strstr(referer_match,host_showad) /* see if this host sees ad */
   == NULL )				/* not mimetex host for ad */
     adfrequency = 0;			/* turn off advertisements */
/* --- check for advertisement directive (\advertisement) --- */
if ( strreplace(expression,"\\advertisement","",0) /*remove \advertisement*/
>=   1 ) adfrequency = 1;		/* force advertisement display */
if ( adfrequency > 0 ) {		/* advertising enabled */
  int	npump = crc16(expression)%16;	/* #times, 0-15, to pump rand() */
  srand(atoi(timestamp(TZDELTA,4)));	/* init rand() with mmddhhmmss */
  while ( npump-- >= 0 ) rand();	/* pre-pump rand() before use */
  if ( (1+rand())%adfrequency == 0 ) {	/* once every adfrequency calls */
    advertisement(expression,adtemplate); } } /*wrap expression in advert*/
/* ---
 * check for image caching (and whether or not caching content type)
 * ----------------------------------------------------------------- */
if ( strstr(expression,"\\counter")  != NULL /* can't cache \counter{} */
||   strstr(expression,"\\input")    != NULL /* can't cache \input{} */
||   strstr(expression,"\\today")    != NULL /* can't cache \today */
||   strstr(expression,"\\calendar") != NULL /* can't cache \calendar */
||   strstr(expression,"\\nocach")   != NULL /* no caching requested */
||   isformdata				/* don't cache user form input */
 ) { iscaching = 0;			/* so turn caching off */
     maxage = 5; }			/* and set max-age to 5 seconds */
if ( strstr(expression,"\\depth")    != NULL ) /* cache content-type lines */
     iscachecontenttype = 1;		/* set flag to cache content-type */
if ( strstr(expression,"\\nodepth")  != NULL ) /* don't cache content-type */
     iscachecontenttype = 0;		/*set flag to not cache content-type*/
if ( isquery )				/* don't cache command-line images */
 if ( iscaching )			/* image caching enabled */
  {
  /* --- set up path to cached image file --- */
  /*char *md5hash = md5str(expression);*/ /*md5 hash of unedited expression*/
  if ( md5hash == NULL )		/* failed for some reason */
    iscaching = 0;			/* so turn off caching */
  else
   {
   strcpy(cachefile,cachepath);		/* start with (relative) path */
   strcat(cachefile,md5hash);		/* add md5 hash of expression */
   strcat(cachefile,".gif");		/* finish with .gif extension */
   gif_outfile = cachefile;		/* signal GIF_Create() to cache */
   /* --- emit mime content-type line --- */
   if ( 0 && isemitcontenttype )	/* now done in emitcache() */
    { fprintf( stdout, "Cache-Control: max-age=%d\n",maxage );
      if ( abs(valign) < 999 )		/* have vertical align */
        fprintf( stdout, "Vertical-Align: %d\n",valign );
      fprintf( stdout, "Content-type: image/gif\n\n" ); }
   /* --- emit cached image if it already exists --- */
   if ( emitcache(cachefile,maxage,valign,0) > 0 ) /* cached image emitted */
    goto end_of_job;			/* so nothing else to do */
   /* --- log caching request --- */
   if ( msglevel >= 1			/* check if logging */
   /*&&   seclevel <= 5*/ )		/* and if logging permitted */
    if ( !isempty(cachelog) )		/* if a logfile is given */
      { char filename[256];		/* construct cachepath/cachelog */
        FILE *filefp=NULL;		/* fopen(filename) */
        strcpy(filename,cachepath);	/* start with (relative) path */
        strcat(filename,cachelog);	/* add cache log filename */
        if ( (filefp=fopen(filename,"a")) /* open cache logfile for append */
        !=   NULL )			/* ignore logging if can't open */
	 { int isreflogged = 0;		/* set true if http_referer logged */
	   fprintf(filefp,"%s                 %s\n", /* timestamp, md5 file */
	    timestamp(TZDELTA,0),cachefile+strlen(cachepath)); /*skip path*/
	   fprintf(filefp,"%s\n",expression); /* expression in filename */
	   if ( !isempty(http_referer) ) /* show referer if we have one */
	      {	int loglen = strlen(dashes);  /* #chars on line in log file*/
		char *refp = http_referer;    /* line to be printed */
		isreflogged = 1;	      /* signal http_referer logged*/
		while ( 1 ) {		      /* printed in parts if needed*/
		  fprintf(filefp,"%.*s\n",loglen,refp); /* print a part */
		  if ( strlen(refp) <= loglen ) break;  /* no more parts */
		  refp += loglen; } }	      /* bump ptr to next part */
	   if ( !isreflogged )		      /* http_referer not logged */
	     fprintf(filefp,"http://none\n"); /* so log dummy referer line */
	   fprintf(filefp,"remote_addr=%s\n", /* and log ip_address... */
	     (!isempty(remote_addr)?remote_addr:"None")); /* ...or "None" */
	   fprintf(filefp,"%s\n",dashes);     /* separator line */
	   fclose(filefp); }		     /* close logfile immediately */
      } /* --- end-of-if(cachelog!=NULL) --- */
   } /* --- end-of-if/else(md5hash==NULL) --- */
  } /* --- end-of-if(iscaching) --- */
/* ---
 * emit copyright, gnu/gpl notice (if "interactive")
 * ------------------------------------------------- */
if ( !isdumpimage && !ispbmpgm )	/* don't mix ascii with image dump */
 if ( (!isquery||isqlogging) && msgfp!=NULL ) { /* called from command line */
   fprintf(msgfp,"%s\n%s\n",copyright1,copyright2); /* display copyright */
   fprintf(msgfp,"Most recent revision: %s\n",REVISIONDATE); /*revision date*/
   } /* --- end-of-if(!isquery...) --- */
/* -------------------------------------------------------------------------
rasterize expression and put a border around it
-------------------------------------------------------------------------- */
/* --- preprocess expression, converting LaTeX constructs for mimeTeX
       (now done above to get \password{} before referer checks) --- */
if ( 0 && expression!=NULL ) {		/* have expression to rasterize */
  expression = mimeprep(expression); }	/* preprocess expression */
/* --- double-check that we actually have an expression to rasterize --- */
if ( expression == NULL ) {		/* nothing to rasterize */
  if ( exitstatus == 0 ) exitstatus = errorstatus; /*signal error to parent*/
  if ( (!isquery||isqlogging) && msgfp!=NULL ) { /*emit error if not query*/
    if ( exitstatus != 0 ) fprintf(msgfp,"Exit code = %d,\n",exitstatus);
    fprintf(msgfp,"No LaTeX expression to rasterize\n"); }
  goto end_of_job; }			/* and then quit */
/* --- rasterize expression --- */
if ( (sp = rasterize(expression,size)) == NULL ) { /* failed to rasterize */
  if ( exitstatus == 0 ) exitstatus = errorstatus; /*signal error to parent*/
  if ( (!isquery||isqlogging) && msgfp!=NULL ) { /*emit error if not query*/
    if ( exitstatus != 0 ) fprintf(msgfp,"Exit code = %d,\n",exitstatus);
    fprintf(msgfp,"Failed to rasterize %.2048s\n",expression); }
  if ( isquery ) {			/* try to display failed expression*/
    char errormsg[4096];		/* buffer for failed expression */
    strcpy(errormsg,			/* init error message */
    "\\red\\fbox{\\begin{gather}"
    "{\\rm~mi\\underline{meTeX~failed~to~render~your~expressi}on}\\\\[5]");
    strcat(errormsg,"{\\rm\\hspace{10}{"); /*render expression as \rm*/
    strcat(errormsg,strdetex(expression,0));/*add detexed expression to msg*/
    strcat(errormsg,"}\\hspace{10}}\\end{gather}}"); /* finish up */
    if ( (sp = rasterize(errormsg,1)) == NULL ) /*couldn't rasterize errmsg*/
      sp = rasterize(			/* so rasterize generic error */
      "\\red\\rm~\\fbox{mimeTeX~failed~to~render\\\\your~expression}",1); }
  if ( sp ==  NULL ) goto end_of_job;	/* re-check for err message failure*/
  magstep = 1;				/* don't magstep error msgs */
  } /* --- end-of-if((sp=rasterize())==NULL) --- */
/* --- magnify entire image here if we need >>bit<<map for pbm output --- */
if ( !isaa || (ispbmpgm && pbmpgmtype<2) ) { /*or use bytemapmag() instead*/
 if ( magstep > 1 && magstep <= 10 ) {	/* magnify entire bitmap image */
  raster *rastmag(), *magrp=NULL;	/* bitmap magnify function */
  int baseline = sp->baseline;		/* original image baseline */
  magrp = rastmag(sp->image,magstep);	/* magnify raster image */
  if ( magrp != NULL ) {		/* succeeded to magnify image */
    delete_raster(sp->image);		/* free original raster image */
    sp->image = magrp;			/*and replace it with magnified one*/
    /* --- adjust parameters --- */
    baseline *= magstep;		/* scale baseline */
    if ( baseline > 0 ) baseline += 1;	/* adjust for no descenders */
    sp->baseline = baseline; }		/*reset baseline of magnified image*/
  magstep = (-1);			/*done, don't also use bytemapmag()*/
  } /* --- end-of-if(magstep) --- */
 } /* --- end-of-if(1||(ispbmpgm&&pbmpgmtype<2)) --- */
/* ---no border requested, but this adjusts width to multiple of 8 bits--- */
if ( issupersampling )			/* no border needed for gifs */
  bp = sp->image;			/* so just extract pixel map */
else					/* for mime xbitmaps must have... */
  bp = border_raster(sp->image,0,0,0,1); /* image width multiple of 8 bits */
sp->image = bitmap_raster = bp;		/* global copy for gif,png output */
raster_width = bp->width; raster_height = bp->height; /* global copy */
raster_baseline = sp->baseline;		/* global copy (not needed) */
if ( sp!=NULL && bp!=NULL ) {		/* have raster */
  valign= raster_baseline -(raster_height -1);/*#pixels for Vertical-Align:*/
  if ( abs(valign) > 255 ) valign = (-9999); } /* sanity check */
if ( ispbmpgm && pbmpgmtype<2 )		/* -g switch or -g1 switch */
  type_pbmpgm(bp,pbmpgmtype,pbm_outfile); /* emit b/w pbm file */
/* -------------------------------------------------------------------------
generate anti-aliased bytemap from (bordered) bitmap
-------------------------------------------------------------------------- */
if ( isaa )				/* we want anti-aliased bitmap */
  {
  /* ---
   * allocate bytemap and colormap as per width*height of bitmap
   * ----------------------------------------------------------- */
  int	nbytes = (raster_width)*(raster_height); /*#bytes for byte,colormap*/
  if ( isss )				/* anti-aliasing by supersampling */
    bytemap_raster = (intbyte *)(bitmap_raster->pixmap); /*bytemap in raster*/
  else					/* need to allocate bytemap */
    if ( aaalgorithm == 0 )		/* anti-aliasing not wanted */
      isaa = 0;				/* so signal no anti-aliasing */
    else				/* anti-aliasing wanted */
      if ( (bytemap_raster = (intbyte *)malloc(nbytes)) /* malloc bytemap */
      ==   NULL ) isaa = 0;		/* reset flag if malloc failed */
  /* ---
   * now generate anti-aliased bytemap and colormap from bitmap
   * ---------------------------------------------------------- */
  if ( isaa )				/*re-check that we're anti-aliasing*/
    {
    /* ---
     * select anti-aliasing algorithm
     * ------------------------------ */
    if ( !isss )			/* generate bytemap for lowpass */
     switch ( aaalgorithm ) {		/* choose antialiasing algorithm */
       default: isaa = 0; break;	/* unrecognized algorithm */
       case 1:				/* 1 for aalowpass() */
	if ( aalowpass(bp,bytemap_raster,grayscale) /*my own lowpass filter*/
	==   0 )  isaa = 0;		/*failed, so turn off anti-aliasing*/
	break;
       case 2:				/*2 for netpbm pnmalias.c algorithm*/
	if ( aapnm(bp,bytemap_raster,grayscale) /* pnmalias.c filter */
	==   0 )  isaa = 0;		/*failed, so turn off anti-aliasing*/
	break;
       case 3:				/*3 for aapnm() based on aagridnum()*/
	if ( aapnmlookup(bp,bytemap_raster,grayscale) /* pnmalias.c filter */
	==   0 )  isaa = 0;		/*failed, so turn off anti-aliasing*/
	break;
       case 4:				/* 4 for aalookup() table lookup */
	if ( aalowpasslookup(bp,bytemap_raster,grayscale) /* aalookup() */
	==   0 )  isaa = 0;		/*failed, so turn off anti-aliasing*/
	break;
       } /* --- end-of-switch(aaalgorithm) --- */
    /* ---
     * emit aalookup() pattern# counts/percents diagnostics
     * ---------------------------------------------------- */
    if ( !isquery && msgfp!=NULL && msglevel>=99 ) { /*emit patternnumcounts*/
     int pcount0=0, pcount1=0;		/* init total w,b center counts */
     for ( ipattern=1; ipattern<=51; ipattern++ ) { /*each possible pattern*/
      if ( ipattern > 1 )		/* ignore all-white squares */
       pcount0 += patternnumcount0[ipattern];  /* bump total white centers */
      pcount1 += patternnumcount1[ipattern]; } /* bump total black centers */
     if ( pcount0+pcount1 > 0 )		/* have pcounts (using aalookup) */
      fprintf(msgfp, "  aalookup() patterns excluding#1 white"
      " (%%'s are in tenths of a percent)...\n");
     for ( ipattern=1; ipattern<=51; ipattern++ ) { /*each possible pattern*/
      int tot = patternnumcount0[ipattern] + patternnumcount1[ipattern];
      if ( tot > 0 )			/* this pattern occurs in image */
       fprintf(msgfp,
       "  pattern#%2d: %7d(%6.2f%%) +%7d(%6.2f%%) =%7d(%6.2f%%)\n",
       ipattern, patternnumcount0[ipattern],  (ipattern<=1? 999.99:
       1000.*((double)patternnumcount0[ipattern])/((double)pcount0)),
       patternnumcount1[ipattern],
       1000.*((double)patternnumcount1[ipattern])/((double)pcount1),
       tot,  (ipattern<=1? 999.99:
       1000.*((double)tot)/((double)(pcount0+pcount1))) ); }
      if ( pcount0+pcount1 > 0 )	/* true when using aalookup() */
       fprintf(msgfp,
       "all patterns: %7d          +%7d          =%7d  total pixels\n",
       pcount0,pcount1,pcount0+pcount1); }
    /* ---
     * apply magstep if requested and not already done to bitmap above
     * --------------------------------------------------------------- */
      if ( 1 ) {			/* or use rastmag() above instead */
       if ( magstep > 1 && magstep <= 10 ) { /*magnify entire bytemap image*/
        intbyte *bytemapmag(), *magmap=NULL; /* bytemap magnify function */
        magmap=bytemapmag(bytemap_raster,raster_width,raster_height,magstep);
        if ( magmap != NULL ) {		/* succeeded to magnify image */
          free(bytemap_raster);		/* free original bytemap image */
          bytemap_raster = magmap;	/*and replace it with magnified one*/
          /* --- adjust parameters --- */
          raster_width *= magstep; raster_height *= magstep; /*scale raster*/
          nbytes *= (magstep*magstep);	/* scale total image size */
          if ( abs(valign) < 255 ) {	/* valign okay */
            valign *= magstep;		/* scale by magstep */
            if ( abs(valign) > 512 ) valign = (-9999); } /* sanity check */
          } /* --- end-of-if(magmap!=NULL) --- */
        magstep = (-1);			/*done, don't also use bytemapmag()*/
        } /* --- end-of-if(magstep) --- */
       } /* --- end-of-if(1) --- */
    /* ---
     * finally, generate colors and colormap
     * ------------------------------------- */
    if ( isaa )				/* have bytemap, so... */
      if ( (colormap_raster = (intbyte *)malloc(nbytes)) /*malloc colormap*/
      ==   NULL ) isaa = 0;		/* reset flag if malloc failed */
    if ( isaa ) {			/* we have byte/colormap_raster */
      ncolors = aacolormap(bytemap_raster,nbytes,colors,colormap_raster);
      if ( ncolors < 2 )		/* failed */
	{ isaa = 0;			/* so turn off anti-aliasing */
	  ncolors = 2; }		/* and reset for black&white */
      } /* --- end-of-if(isaa) --- */
     if ( isaa && ispbmpgm && pbmpgmtype>1 ) { /* -g2 switch  */
      raster pbm_raster;		/*construct arg for write_pbmpgm()*/
      pbm_raster.width  = raster_width;  pbm_raster.height = raster_height;
      pbm_raster.pixsz  = 8;  pbm_raster.pixmap = (pixbyte *)bytemap_raster;
      type_pbmpgm(&pbm_raster,pbmpgmtype,pbm_outfile); } /*grayscale file*/
    } /* --- end-of-if(isaa) --- */
  } /* --- end-of-if(isaa) --- */
/* -------------------------------------------------------------------------
display results on msgfp if called from command line (usually for testing)
-------------------------------------------------------------------------- */
if ( (!isquery||isqlogging) || msglevel >= 99 )	/*command line or debuging*/
 if ( !isdumpimage && !ispbmpgm )	/* don't mix ascii with image dump */
  {
  /* ---
   * display ascii image of rasterize()'s rasterized bitmap
   * ------------------------------------------------------ */
  if ( !isss )				/* no bitmap for supersampling */
    { fprintf(msgfp,"\nAscii dump of bitmap image...\n");
      type_raster(bp,msgfp); }		/* emit ascii image of raster */
  /* ---
   * display anti-aliasing results applied to rasterized bitmap
   * ---------------------------------------------------------- */
  if ( isaa )				/* if anti-aliasing applied */
    {
    int	igray;				/* colors[] index */
    /* --- anti-aliased bytemap image --- */
    if ( msgfp!=NULL && msglevel>=9 )	/* don't usually emit raw bytemap */
      {	fprintf(msgfp,"\nHex dump of anti-aliased bytemap, " /*emit bytemap*/
	"asterisks denote \"black\" bytes (value=%d)...\n",grayscale-1);
	type_bytemap(bytemap_raster,grayscale,
        raster_width,raster_height,msgfp); }
    /* --- colormap image --- */
    fprintf(msgfp,"\nHex dump of colormap indexes, "  /* emit colormap */
      "asterisks denote \"black\" bytes (index=%d)...\n",ncolors-1);
    type_bytemap(colormap_raster,ncolors,
    raster_width,raster_height,msgfp);
    /* --- rgb values corresponding to colormap indexes */
    fprintf(msgfp,"\nThe %d colormap indexes denote rgb values...",ncolors);
    for ( igray=0; igray<ncolors; igray++ ) /* show colors[] values */
      fprintf(msgfp,"%s%2x-->%3d", (igray%5?"   ":"\n"),
	igray,(int)(colors[ncolors-1]-colors[igray]));
    fprintf(msgfp,"\n");		/* always needs a final newline */
    } /* --- end-of-if(isaa) --- */
  } /* --- end-of-if(!isquery||msglevel>=9) --- */
/* -------------------------------------------------------------------------
emit xbitmap or gif image, and exit
-------------------------------------------------------------------------- */
if (  (isquery     && !ispbmpgm)	/* called from browser (usual) */
||    (isdumpimage && !ispbmpgm)	/* or to emit gif dump of image */
||    (msglevel    >= 99) )		/* or for debugging */
 {
 int  igray = 0;			/* grayscale index */
 #if defined(GIF)			/* compiled to emit gif */
 /* ------------------------------------------------------------------------
 emit GIF image
 ------------------------------------------------------------------------- */
  /* --- don't use memory buffer if outout file given --- */
  if ( gif_outfile != NULL ) isinmemory = 0; /* reset memory buffer flag */
  /* --- construct contenttype[] buffer containing mime headers --- */
  if ( 1 ) {				/* always construct buffer */
    sprintf( contenttype, "Cache-Control: max-age=%d\n", maxage );
    /*sprintf(contenttype+strlen(contenttype),
       "Expires: Fri, 31 Oct 2003 23:59:59 GMT\n" );*/
    /*sprintf(contenttype+strlen(contenttype),
       "Last-Modified: Wed, 15 Oct 2003 01:01:01 GMT\n");*/
    if ( abs(valign) < 999 )		/* have Vertical-Align: header info*/
      sprintf( contenttype+strlen(contenttype),
       "Vertical-Align: %d\n", valign );
    sprintf( contenttype+strlen(contenttype),
      "Content-type: image/gif\n\n" ); }
  /* --- emit mime content-type line --- */
  if ( isemitcontenttype		/* content-type lines wanted */
  &&   !isdumpimage			/* don't mix ascii with image dump */
  &&   !isinmemory			/* done below if in memory */
  &&   !iscaching )			/* done by emitcache() if caching */
    { fputs(contenttype,stdout); }	/* emit content-type: header buffer*/
  /* --- write output to memory buffer, possibly for testing --- */
  if ( isinmemory			/* want gif written to memory */
  ||   isdumpbuffer )			/*or dump memory buffer for testing*/
   if ( gif_outfile == NULL )		/* and don't already have a file */
    { *gif_buffer = '\000';		/* init buffer as empty string */
      memset(gif_buffer,0,MAXGIFSZ);	/* zero out buffer */
      gif_outfile = gif_buffer;		/* and point outfile to buffer */
      if ( isdumpbuffer )		/* buffer dump test requested */
	isdumpbuffer = 999; }		/* so signal dumping to buffer */
  /* --- initialize gifsave library and colors --- */
  if ( msgfp!=NULL && msglevel>=999 )
    { fprintf(msgfp,"main> calling GIF_Create(*,%d,%d,%d,8)\n",
      raster_width,raster_height,ncolors); fflush(msgfp); }
  while ( 1 )		/* init gifsave lib, and retry if caching fails */
    { int status = GIF_Create(gif_outfile,
        raster_width,raster_height, ncolors, 8);
      if ( status == 0 ) break;		/* continue if succeeded */
      if ( iscaching == 0 ) goto end_of_job; /* quit if failed */
      iscaching = 0;			/* retry without cache file */
      isdumpbuffer = 0;			/* reset isdumpbuffer signal */
      if ( isquery ) isinmemory = 1;	/* force in-memory image generation*/
      if ( isinmemory ) {		/* using memory buffer */
	gif_outfile = gif_buffer;	/* emit images to memory buffer */
	*gif_outfile = '\000'; }	/* empty string signals buffer */
      else {				/* or */
	gif_outfile = (char *)NULL;	/* emit images to stdout */
	if ( isemitcontenttype ) {	/* content-type lines wanted */
	  fprintf( stdout, "Cache-Control: max-age=%d\n",maxage );
	  fprintf( stdout, "Content-type: image/gif\n\n" ); } }
    } /* --- end-of-while(1) --- */
  GIF_SetColor(0,bgred,bggreen,bgblue);	/* background white if all 255 */
  if ( !isaa )				/* just b&w if not anti-aliased */
    { GIF_SetColor(1,fgred,fggreen,fgblue); /* foreground black if all 0 */
      colors[0]='\000'; colors[1]='\001'; } /* and set 2 b&w color indexes */
  else					/* set grayscales for anti-aliasing */
    /* --- anti-aliased, so call GIF_SetColor() for each colors[] --- */
    for ( igray=1; igray<ncolors; igray++ ) /* for colors[] values */
      {
      /*--- gfrac goes from 0 to 1.0, as igray goes from 0 to ncolors-1 ---*/
      double gfrac = ((double)colors[igray])/((double)colors[ncolors-1]);
      /* --- r,g,b components go from background to foreground color --- */
      int red  = iround(((double)bgred)  +gfrac*((double)(fgred-bgred))),
	  green= iround(((double)bggreen)+gfrac*((double)(fggreen-bggreen))),
	  blue = iround(((double)bgblue) +gfrac*((double)(fgblue-bgblue)));
      /* --- set color index number igray to rgb values gray,gray,gray --- */
      GIF_SetColor(igray, red,green,blue); /*set gray,grayer,...,0=black*/
      } /* --- end-of-for(igray) --- */
  /* --- set gif color#0 (background) transparent --- */
  if ( istransparent )			/* transparent background wanted */
    GIF_SetTransparent(0);		/* set transparent background */
  if (msgfp!=NULL && msglevel>=9) fflush(msgfp); /*flush debugging output*/
  /* --- emit compressed gif image (to stdout or cache file) --- */
  GIF_CompressImage(0, 0, -1, -1, GetPixel); /* emit gif */
  GIF_Close();				/* close file */
  if ( msgfp!=NULL && msglevel>=9 )
    { fprintf(msgfp,"main> created gifSize=%d\n", gifSize);
      fflush(msgfp); }
  /* --- may need to emit image from cached file or from memory --- */
  if ( isquery				/* have an actual query string */
  ||   isdumpimage			/* or dumping image */
  ||   msglevel >= 99 ) {		/* or debugging */
  int maxage2 = (isdumpimage?(-1):maxage); /* no headers if dumping image */
   if ( iscaching )			/* caching enabled */
     emitcache(cachefile,maxage2,valign,0); /*emit cached image (hopefully)*/
   else if ( isinmemory )		/* or emit image from memory buffer*/
     emitcache(gif_buffer,maxage2,valign,1); } /*emitted from memory buffer*/
  /* --- for testing, may need to write image buffer to file --- */
  if ( isdumpbuffer > 99 )		/* gif image in memory buffer */
   if ( gifSize > 0 )			/* and it's not an empty buffer */
    { FILE *dumpfp = fopen("mimetex.gif","wb"); /* dump to mimetex.gif */
      if ( dumpfp != NULL )		/* file opened successfully */
	{ fwrite(gif_buffer,sizeof(unsigned char),gifSize,dumpfp); /*write*/
	  fclose(dumpfp); }		/* and close file */
    } /* --- end-of-if(isdumpbuffer>99) --- */
 #else
 /* ------------------------------------------------------------------------
 emit mime XBITMAP image
 ------------------------------------------------------------------------- */
  xbitmap_raster(bp,stdout);		/* default emits mime xbitmap */
 #endif
 } /* --- end-of-if(isquery) --- */
/* --- exit --- */
end_of_job:
  if ( !isss )				/*bytemap raster in sp for supersamp*/
   if ( bytemap_raster != NULL ) free(bytemap_raster);/*free bytemap_raster*/
  if (colormap_raster != NULL )free(colormap_raster); /*and colormap_raster*/
  if ( 0 && gif_buffer != NULL ) free(gif_buffer); /* free malloced buffer */
  if ( 1 && sp != NULL ) delete_subraster(sp);	/* and free expression */
  if ( msgfp != NULL			/* have message/log file open */
  &&   msgfp != stdout )		/* and it's not stdout */
   { fprintf(msgfp,"mimeTeX> successful end-of-job at %s\n",
       timestamp(TZDELTA,0));
     fprintf(msgfp,"%s\n",dashes);	/* so log separator line */
     fclose(msgfp); }			/* and close logfile */
  /* --- dump memory leaks in debug window if in MS VC++ debug mode --- */
  #if defined(_CRTDBG_MAP_ALLOC)
    _CrtDumpMemoryLeaks();
  #endif
  /* --- exit() if not running as Windows DLL (see CreateGifFromEq()) --- */
  #if !defined(_USRDLL)
    if ( errorstatus == 0 )		/*user doesn't want errors signalled*/
      exitstatus = 0;			/* so reset error status */
    exit ( exitstatus );
  #endif
} /* --- end-of-function main() --- */

/* ==========================================================================
 * Function:	CreateGifFromEq ( expression, gifFileName )
 * Purpose:	shortcut method to create GIF file for expression,
 *		with antialising and all other capabilities
 * --------------------------------------------------------------------------
 * Arguments:	expression (I)	char *ptr to null-terminated string
 *				containing LaTeX expression to be rendred
 *		gifFileName (I)	char *ptr to null-terminated string
 *				containing name of output gif file
 * --------------------------------------------------------------------------
 * Returns:	( int )		exit value from main (0 if successful)
 * --------------------------------------------------------------------------
 * Notes:     o	This function is the entry point when mimeTeX is built
 *		as a Win32 DLL rather then a standalone app or CGI
 *	      o	Contributed to mimeTeX by Shital Shah.  See his homepage
 *		  http://www.shitalshah.com
 *	      o	Shital discusses the mimeTeX Win32 DLL project at
 *		  http://www.codeproject.com/dotnet/Eq2Img.asp
 *		and you can download his latest code from
 *		  http://www.shitalshah.com/dev/eq2img_all.zip
 * ======================================================================= */
/* --- include function to expose Win32 DLL to outside world --- */
#if defined(_USRDLL)
  extern _declspec(dllexport)int _cdecl
	CreateGifFromEq ( char *expression, char *gifFileName );
#endif
/* --- entry point --- */
int	CreateGifFromEq ( char *expression, char *gifFileName )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
int	main();			/* main() akways returns an int */
/* --- set constants --- */
int	argc = 4;		/* count of args supplied to main() */
char	*argv[5] =		/* command line args to run with -e option */
	  { "MimeTeXWin32DLL", "-e", /* constant args */
	    /*gifFileName, expression,*/ NULL, NULL, NULL };
/* --- set argv[]'s not computable at load time --- */
argv[2] = gifFileName;		/* args are -e gifFileName */
argv[3] = expression;		/* and now  -e gifFileName expression */
/* -------------------------------------------------------------------------
Run mimeTeX in command-line mode with -e (export) option, and then return
-------------------------------------------------------------------------- */
return	main ( argc, argv
	  #ifdef DUMPENVP
	    , NULL
	  #endif
	) ;
} /* --- end-of-function CreateGifFromEq() --- */


/* ==========================================================================
 * Function:	ismonth ( char *month )
 * Purpose:	returns 1 if month contains current month "jan"..."dec".
 * --------------------------------------------------------------------------
 * Arguments:	month (I)	char * containing null-terminated string
 *				in which "jan"..."dec" is (putatively)
 *				contained as a substring.
 * --------------------------------------------------------------------------
 * Returns:	( int )		1 if month contains current month,
 *				0 otherwise
 * --------------------------------------------------------------------------
 * Notes:     o	There's a three day "grace period", e.g., Dec 3 mtaches Nov.
 * ======================================================================= */
/* --- entry point --- */
int	ismonth ( char *month )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
int	isokay = 0;			/*1 if month contains current month*/
/*long	time_val = 0L;*/		/* binary value returned by time() */
time_t	time_val = (time_t)(0);		/* binary value returned by time() */
struct tm *tmstruct=(struct tm *)NULL, *localtime(); /* interpret time_val */
int	imonth, mday;			/* current month 1-12 and day 1-31 */
int	ngrace = 3;			/* grace period */
char	lcmonth[128]="\000"; int i=0;	/* lowercase month */
static	char *months[] =		/* month must contain current one */
   {"dec","jan","feb","mar","apr","may","jun",
    "jul","aug","sep","oct","nov","dec","jan"};
/* -------------------------------------------------------------------------
get current date:time info, and check month
-------------------------------------------------------------------------- */
/* --- lowercase input month --- */
if ( month != NULL )			/* check that we got input */
  for ( i=0; i<120 && *month!='\000'; i++,month++ ) /* go thru month chars */
    lcmonth[i] = tolower(*month);	/* lowerase each char in month */
if ( i < 2 ) goto end_of_job;		/* must be invalid input */
lcmonth[i] = '\000';			/* null-terminate lcmonth[] */
/* --- get current date:time --- */
time((time_t *)(&time_val));		/* get date and time */
tmstruct = localtime((time_t *)(&time_val)); /* interpret time_val */
/* --- month and day  --- */
imonth = 1 + (int)(tmstruct->tm_mon);	/* 1=jan ... 12=dec */
mday = (int)(tmstruct->tm_mday);	/* 1-31 */
if ( imonth<1 || imonth>12		/* quit if month out-of-range */
||   mday<0 || mday>31 ) goto end_of_job; /* or date out of range */
/* --- check input month against current date --- */
if ( strstr(lcmonth,months[imonth]) != NULL ) isokay = 1; /* current month */
if ( mday <= ngrace )			/* 1-3 within grace period */
 if ( strstr(lcmonth,months[imonth-1]) != NULL ) isokay = 1; /* last month */
if ( mday >= 31-ngrace )		/* 28-31 within grace period */
 if ( strstr(lcmonth,months[imonth+1]) != NULL ) isokay = 1; /* next month */
end_of_job:
  return ( isokay );			/*1 if month contains current month*/
} /* --- end-of-function ismonth() --- */


/* ==========================================================================
 * Function:	logger ( logfp, msglevel, message, logvars )
 * Purpose:	Logs the environment variables specified in logvars
 *		to fp if their msglevel is >= the passed msglevel.
 * --------------------------------------------------------------------------
 * Arguments:	logfp (I)	FILE * to file containing log,
 *				or NULL defaults to global msgfp
 *		msglevel (I)	int containing logging message level
 *		message (I)	char * to optional message, or NULL
 *		logvars (I)	logdata * to array of environment variables
 *				to be logged
 * --------------------------------------------------------------------------
 * Returns:	( int )		number of variables from logvars
 *				that were actually logged
 * --------------------------------------------------------------------------
 * Notes:     o
 * ======================================================================= */
/* --- entry point --- */
int	logger ( FILE *logfp, int msglevel, char *message, logdata *logvars )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
FILE	*fp = (logfp!=NULL?logfp:msgfp); /* use caller's logfp, or msgfp */
int	ilog=0, nlogged=0;		/* logvars[] index, #vars logged */
char	*timestamp();			/* timestamp logged */
char	*value = NULL;			/* getenv(name) to be logged */
static	int  ncalls = 0;		/* #calls (headers on 1st call) */
static	char *dashes =
  "------------------------------------------------------------------------";
/* -------------------------------------------------------------------------
Log each variable
-------------------------------------------------------------------------- */
if ( fp == NULL ) goto end_of_job;	/* no output file */
if ( ++ncalls == 1 ) {			/* headers */
  fprintf(fp,"%s\n",dashes);
  fprintf(fp,"%s\n",timestamp(TZDELTA,0)); } /*emit timestamp before 1st var*/
if ( !isempty(message) )		/* optional message supplied */
 fprintf(fp,"  MESSAGE = %s\n",message); /* emit caller-supplied message */
if ( logvars != (logdata *)NULL )	/* have logvars */
 for ( ilog=0; logvars[ilog].name != NULL; ilog++ )  /* till end-of-table */
  if ( msglevel >= logvars[ilog].msglevel ) /* check msglevel for this var */
   if ( (value=getenv(logvars[ilog].name))  /* getenv(name) to be logged */
   != NULL )				/* check that name exists */
    {
    fprintf(fp,"  %s = %.*s\n",		/* emit variable name = value */
     logvars[ilog].name,logvars[ilog].maxlen,value);
    nlogged++;				/* bump #vars logged */
    } /* --- end-of-for(ilog) --- */
end_of_job: return ( nlogged );		/* back to caller */
} /* --- end-of-function logger() --- */


/* ==========================================================================
 * Function:	emitcache ( cachefile, maxage, valign, isbuffer )
 * Purpose:	dumps bytes from cachefile to stdout
 * --------------------------------------------------------------------------
 * Arguments:	cachefile (I)	pointer to null-terminated char string
 *				containing full path to file to be dumped,
 *				or contains buffer of bytes to be dumped
 *		maxage (I)	int containing maxage, in seconds, for
 *				http header, or -1 to not emit headers
 *		valign (I)	int containing Vertical-Align:, in pixels,
 *				for http header, or <= -999 to not emit
 *		isbuffer (I)	1 if cachefile is buffer of bytes to be
 *				dumped
 * --------------------------------------------------------------------------
 * Returns:	( int )		#bytes dumped (0 signals error)
 * --------------------------------------------------------------------------
 * Notes:     o
 * ======================================================================= */
/* --- entry point --- */
int	emitcache ( char *cachefile, int maxage, int valign, int isbuffer )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
int	nbytes=gifSize, readcachefile(); /* read cache file */
FILE	*emitptr = stdout;		/* emit cachefile to stdout */
unsigned char buffer[MAXGIFSZ+1];	/* bytes from cachefile */
unsigned char *buffptr = buffer;	/* ptr to buffer */
int	isvalign = (abs(valign)<999?1:0); /* true to emit Vertical-Align: */
int	iscontenttypecached = iscachecontenttype; /*true if headers cached*/
/* -------------------------------------------------------------------------
initialization
-------------------------------------------------------------------------- */
/* --- check that files opened okay --- */
if ( emitptr == (FILE *)NULL )		/* failed to open emit file */
  goto end_of_job;			/* so return 0 bytes to caller */
/* --- read the file if necessary --- */
if ( isbuffer ) {			/* cachefile is buffer */
  buffptr = (unsigned char *)cachefile;	/* so reset buffer pointer */
  iscontenttypecached = 0; }		/* and iscontenttypecached flag */
else {					/* cachefile is file name */
  if ( (nbytes = readcachefile(cachefile,buffer)) /* read the file */
  < 1 ) goto end_of_job; }		/* quit if file not read */
/* --- first emit http headers if requested --- */
if ( isemitcontenttype			/* content-type lines enabled */
&&   !iscontenttypecached		/* and not in cached image */
&&   maxage >= 0 )			/* caller wants http headers */
 { /* --- emit mime content-type line --- */
   fprintf( emitptr, "Cache-Control: max-age=%d\n",maxage );
   fprintf( emitptr, "Content-Length: %d\n",nbytes );
   if ( isvalign )			/* Vertical-Align: header wanted */
     fprintf( emitptr, "Vertical-Align: %d\n",valign );
   fprintf( emitptr, "Content-type: image/gif\n\n" ); }
/* -------------------------------------------------------------------------
set stdout to binary mode (for Windows)
-------------------------------------------------------------------------- */
/* emitptr = fdopen(STDOUT_FILENO,"wb"); */  /* doesn't work portably, */
#ifdef WINDOWS				/* so instead... */
  #ifdef HAVE_SETMODE			/* prefer (non-portable) setmode() */
    if ( setmode ( fileno (stdout), O_BINARY) /* windows specific call */
    == -1 ) ; /* handle error */	/* sets stdout to binary mode */
  #else					/* setmode() not available */
    #if 1
      freopen ("CON", "wb", stdout);	/* freopen() stdout binary */
    #else
      stdout = fdopen (STDOUT_FILENO, "wb"); /* fdopen() stdout binary */
    #endif
  #endif
#endif
/* -------------------------------------------------------------------------
emit bytes from cachefile
-------------------------------------------------------------------------- */
/* --- write bytes to stdout --- */
if ( fwrite(buffptr,sizeof(unsigned char),nbytes,emitptr) /* write buffer */
<    nbytes )				/* failed to write all bytes */
  nbytes = 0;				/* reset total count to 0 */
if ( isdumpgif && (0 || !isbuffer) ) {	/* **debug** dump gif */
  char dumpfile[999];  FILE *dumpptr=NULL;
  strcpy(dumpfile,cachefile);
  strcat(dumpfile,"_"); strcat(dumpfile,DUMPGIF);
  if ( (dumpptr=fopen(dumpfile,"wb")) != NULL ) {
    fwrite(buffptr,sizeof(unsigned char),nbytes,dumpptr);
    fclose(dumpptr); }
  } /* --- end-of-if(isdumpgif) --- */
end_of_job:
  return ( nbytes );			/* back with #bytes emitted */
} /* --- end-of-function emitcache() --- */


/* ==========================================================================
 * Function:	readcachefile ( cachefile, buffer )
 * Purpose:	read cachefile into buffer
 * --------------------------------------------------------------------------
 * Arguments:	cachefile (I)	pointer to null-terminated char string
 *				containing full path to file to be read
 *		buffer (O)	pointer to unsigned char string
 *				returning contents of cachefile
 *				(max 64000 bytes)
 * --------------------------------------------------------------------------
 * Returns:	( int )		#bytes read (0 signals error)
 * --------------------------------------------------------------------------
 * Notes:     o
 * ======================================================================= */
/* --- entry point --- */
int	readcachefile ( char *cachefile, unsigned char *buffer )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
FILE	*cacheptr = fopen(cachefile,"rb"); /*open cachefile for binary read*/
unsigned char cachebuff[64];		/* bytes from cachefile */
int	buflen = 32,			/* #bytes we try to read from file */
	nread = 0,			/* #bytes actually read from file */
	maxbytes = MAXGIFSZ,		/* max #bytes returned in buffer */
	nbytes = 0;			/* total #bytes read */
/* -------------------------------------------------------------------------
initialization
-------------------------------------------------------------------------- */
/* --- check that files opened okay --- */
if ( cacheptr == (FILE *)NULL ) goto end_of_job; /*failed to open cachefile*/
/* --- check that output buffer provided --- */
if ( buffer == (unsigned char *)NULL ) goto end_of_job; /* no buffer */
/* -------------------------------------------------------------------------
read bytes from cachefile
-------------------------------------------------------------------------- */
while ( 1 )
  {
  /* --- read bytes from cachefile --- */
  nread = fread(cachebuff,sizeof(unsigned char),buflen,cacheptr); /* read */
  if ( nbytes + nread > maxbytes )	/* block too big for buffer */
    nread = maxbytes - nbytes;		/* so truncate it */
  if ( nread < 1 ) break;		/* no bytes left in cachefile */
  /* --- store bytes in buffer --- */
  memcpy(buffer+nbytes,cachebuff,nread); /* copy current block to buffer */
  /* --- ready to read next block --- */
  nbytes += nread;			/* bump total #bytes emitted */
  if ( nread < buflen ) break;		/* no bytes left in cachefile */
  if ( nbytes >= maxbytes ) break;	/* avoid buffer overflow */
  } /* --- end-of-while(1) --- */
end_of_job:
  if ( cacheptr != NULL ) fclose(cacheptr); /* close file if opened */
  return ( nbytes );			/* back with #bytes emitted */
} /* --- end-of-function readcachefile() --- */


/* ==========================================================================
 * Function:	advertisement ( expression, message )
 * Purpose:	wrap expression in advertisement message
 * --------------------------------------------------------------------------
 * Arguments:	expression (I/O) pointer to null-terminated char string
 *				containing expression to be "wrapped",
 *				and returning wrapped expression
 *		message (I)	pointer to null-terminated char string
 *				containing template for advertisement
 *				message, or NULL to use default message
 * --------------------------------------------------------------------------
 * Returns:	( int )		1 if successful, 0=error
 * --------------------------------------------------------------------------
 * Notes:     o
 * ======================================================================= */
/* --- entry point --- */
int	advertisement ( char *expression, char *message )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
/* --- advertisement template --- */
char  *adtemplate =
	#if defined(ADVERTISEMENT)	/* cc -DADVERTISEMENT=\"filename\" */
	  #include ADVERTISEMENT	/* filename with advertisement */
	#else				/* formatted as illustrated below */
	"\\begin{gather} {\\small\\text \\fbox{\\begin{gather}"
	"mime\\TeX rendering courtesy of\\\\"
	"\\homepagetext \\end{gather}}}\\\\"
	" %%beginmath%% %%expression%% %%endmath%% \\end{gather}"
	#endif
	;				/* terminating semicolon */
/* --- other variables --- */
char	adbuffer[MAXEXPRSZ+2048];	/*construct wrapped expression here*/
char	*beginmath = " ",		/* start math mode */
	*endmath =   " ";		/* end math mode */
int	strreplace();			/* replace %%keywords%% with values*/
/* -------------------------------------------------------------------------
wrap expression in advertisement
-------------------------------------------------------------------------- */
/* --- start with template --- */
if ( isempty(message) )			/* caller didn't supply message */
  message = adtemplate;			/* so use default message */
strcpy(adbuffer,message);		/* copy message template to buffer */
/* --- replace %%beginmath%%...%%endmath%% --- */
  strreplace(adbuffer,"%%beginmath%%",beginmath,0);
  strreplace(adbuffer,"%%endmath%%",endmath,0);
/* --- replace %%expression%% in template with expression --- */
  strreplace(adbuffer,"%%expression%%",expression,0);
/* --- replace original expression --- */
strcpy(expression,adbuffer);		/* expression now wrapped in ad */
return ( 1 );				/* always just return 1 */
} /* --- end-of-function advertisement() --- */


/* ==========================================================================
 * Function:	crc16 ( s )
 * Purpose:	16-bit crc of string s
 * --------------------------------------------------------------------------
 * Arguments:	s (I)		pointer to null-terminated char string
 *				whose crc is desired
 * --------------------------------------------------------------------------
 * Returns:	( int )		16-bit crc of s
 * --------------------------------------------------------------------------
 * Notes:     o	From Numerical Recipes in C, 2nd ed, page 900.
 * ======================================================================= */
/* --- entry point --- */
int	crc16 ( char *s )
{
/* -------------------------------------------------------------------------
Compute the crc
-------------------------------------------------------------------------- */
unsigned short crc = 0;			/* returned crc */
int	ibit;				/* for(ibit) eight one-bit shifts */
while ( !isempty(s) ) {			/* while there are still more chars*/
  crc = (crc ^ (*s)<<8);		/* add next char */
  for ( ibit=0; ibit<8; ibit++ )	/* generator polynomial */
    if ( crc & 0x8000 ) { crc<<=1; crc=crc^4129; }
    else crc <<= 1;
  s++;					/* next xhar */
  } /* --- end-of-while(!isempty(s)) --- */
return ( (int)crc );			/* back to caller with crc */
} /* --- end-of-function crc16() --- */


/* ==========================================================================
 * Function:	md5str ( instr )
 * Purpose:	returns null-terminated character string containing
 *		md5 hash of instr (input string)
 * --------------------------------------------------------------------------
 * Arguments:	instr (I)	pointer to null-terminated char string
 *				containing input string whose md5 hash
 *				is desired
 * --------------------------------------------------------------------------
 * Returns:	( char * )	ptr to null-terminated 32-character
 *				md5 hash of instr
 * --------------------------------------------------------------------------
 * Notes:     o	Other md5 library functions are included below.
 *		They're all taken from Christophe Devine's code,
 *		which (as of 04-Aug-2004) is available from
 *		     http://www.cr0.net:8040/code/crypto/md5/
 *	      o	The P,F,S macros in the original code are replaced
 *		by four functions P1()...P4() to accommodate a problem
 *		with Compaq's vax/vms C compiler.
 * ======================================================================= */
/* --- #include "md5.h" --- */
#ifndef uint8
  #define uint8  unsigned char
#endif
#ifndef uint32
  #define uint32 unsigned long int
#endif
typedef struct
  { uint32 total[2];
    uint32 state[4];
    uint8 buffer[64];
  } md5_context;
void md5_starts( md5_context *ctx );
void md5_update( md5_context *ctx, uint8 *input, uint32 length );
void md5_finish( md5_context *ctx, uint8 digest[16] );
/* --- md5.h --- */
#define GET_UINT32(n,b,i)                       \
  { (n) = ( (uint32) (b)[(i)    ]       )       \
        | ( (uint32) (b)[(i) + 1] <<  8 )       \
        | ( (uint32) (b)[(i) + 2] << 16 )       \
        | ( (uint32) (b)[(i) + 3] << 24 ); }
#define PUT_UINT32(n,b,i)                       \
  { (b)[(i)    ] = (uint8) ( (n)       );       \
    (b)[(i) + 1] = (uint8) ( (n) >>  8 );       \
    (b)[(i) + 2] = (uint8) ( (n) >> 16 );       \
    (b)[(i) + 3] = (uint8) ( (n) >> 24 ); }
/* --- P,S,F macros defined as functions --- */
void P1(uint32 *X,uint32 *a,uint32 b,uint32 c,uint32 d,int k,int s,uint32 t)
  { *a += (uint32)(d ^ (b & (c ^ d))) + X[k] + t;
    *a  = ((*a<<s) | ((*a & 0xFFFFFFFF) >> (32-s))) + b;
    return; }
void P2(uint32 *X,uint32 *a,uint32 b,uint32 c,uint32 d,int k,int s,uint32 t)
  { *a += (uint32)(c ^ (d & (b ^ c))) + X[k] + t;
    *a  = ((*a<<s) | ((*a & 0xFFFFFFFF) >> (32-s))) + b;
    return; }
void P3(uint32 *X,uint32 *a,uint32 b,uint32 c,uint32 d,int k,int s,uint32 t)
  { *a += (uint32)(b ^ c ^ d) + X[k] + t;
    *a  = ((*a<<s) | ((*a & 0xFFFFFFFF) >> (32-s))) + b;
    return; }
void P4(uint32 *X,uint32 *a,uint32 b,uint32 c,uint32 d,int k,int s,uint32 t)
  { *a += (uint32)(c ^ (b | ~d)) + X[k] + t;
    *a  = ((*a<<s) | ((*a & 0xFFFFFFFF) >> (32-s))) + b;
    return; }

/* --- entry point (this one little stub written by me)--- */
char *md5str( char *instr )
  { static char outstr[64];
    unsigned char md5sum[16];
    md5_context ctx;
    int j;
    md5_starts( &ctx );
    md5_update( &ctx, (uint8 *)instr, strlen(instr) );
    md5_finish( &ctx, md5sum );
    for( j=0; j<16; j++ )
      sprintf( outstr + j*2, "%02x", md5sum[j] );
    outstr[32] = '\000';
    return ( outstr ); }

/* --- entry point (all md5 functions below by Christophe Devine) --- */
void md5_starts( md5_context *ctx )
  { ctx->total[0] = 0;
    ctx->total[1] = 0;
    ctx->state[0] = 0x67452301;
    ctx->state[1] = 0xEFCDAB89;
    ctx->state[2] = 0x98BADCFE;
    ctx->state[3] = 0x10325476; }

void md5_process( md5_context *ctx, uint8 data[64] )
  { uint32 X[16], A, B, C, D;
    GET_UINT32( X[0],  data,  0 );
    GET_UINT32( X[1],  data,  4 );
    GET_UINT32( X[2],  data,  8 );
    GET_UINT32( X[3],  data, 12 );
    GET_UINT32( X[4],  data, 16 );
    GET_UINT32( X[5],  data, 20 );
    GET_UINT32( X[6],  data, 24 );
    GET_UINT32( X[7],  data, 28 );
    GET_UINT32( X[8],  data, 32 );
    GET_UINT32( X[9],  data, 36 );
    GET_UINT32( X[10], data, 40 );
    GET_UINT32( X[11], data, 44 );
    GET_UINT32( X[12], data, 48 );
    GET_UINT32( X[13], data, 52 );
    GET_UINT32( X[14], data, 56 );
    GET_UINT32( X[15], data, 60 );
    A = ctx->state[0];
    B = ctx->state[1];
    C = ctx->state[2];
    D = ctx->state[3];
    P1( X, &A, B, C, D,  0,  7, 0xD76AA478 );
    P1( X, &D, A, B, C,  1, 12, 0xE8C7B756 );
    P1( X, &C, D, A, B,  2, 17, 0x242070DB );
    P1( X, &B, C, D, A,  3, 22, 0xC1BDCEEE );
    P1( X, &A, B, C, D,  4,  7, 0xF57C0FAF );
    P1( X, &D, A, B, C,  5, 12, 0x4787C62A );
    P1( X, &C, D, A, B,  6, 17, 0xA8304613 );
    P1( X, &B, C, D, A,  7, 22, 0xFD469501 );
    P1( X, &A, B, C, D,  8,  7, 0x698098D8 );
    P1( X, &D, A, B, C,  9, 12, 0x8B44F7AF );
    P1( X, &C, D, A, B, 10, 17, 0xFFFF5BB1 );
    P1( X, &B, C, D, A, 11, 22, 0x895CD7BE );
    P1( X, &A, B, C, D, 12,  7, 0x6B901122 );
    P1( X, &D, A, B, C, 13, 12, 0xFD987193 );
    P1( X, &C, D, A, B, 14, 17, 0xA679438E );
    P1( X, &B, C, D, A, 15, 22, 0x49B40821 );
    P2( X, &A, B, C, D,  1,  5, 0xF61E2562 );
    P2( X, &D, A, B, C,  6,  9, 0xC040B340 );
    P2( X, &C, D, A, B, 11, 14, 0x265E5A51 );
    P2( X, &B, C, D, A,  0, 20, 0xE9B6C7AA );
    P2( X, &A, B, C, D,  5,  5, 0xD62F105D );
    P2( X, &D, A, B, C, 10,  9, 0x02441453 );
    P2( X, &C, D, A, B, 15, 14, 0xD8A1E681 );
    P2( X, &B, C, D, A,  4, 20, 0xE7D3FBC8 );
    P2( X, &A, B, C, D,  9,  5, 0x21E1CDE6 );
    P2( X, &D, A, B, C, 14,  9, 0xC33707D6 );
    P2( X, &C, D, A, B,  3, 14, 0xF4D50D87 );
    P2( X, &B, C, D, A,  8, 20, 0x455A14ED );
    P2( X, &A, B, C, D, 13,  5, 0xA9E3E905 );
    P2( X, &D, A, B, C,  2,  9, 0xFCEFA3F8 );
    P2( X, &C, D, A, B,  7, 14, 0x676F02D9 );
    P2( X, &B, C, D, A, 12, 20, 0x8D2A4C8A );
    P3( X, &A, B, C, D,  5,  4, 0xFFFA3942 );
    P3( X, &D, A, B, C,  8, 11, 0x8771F681 );
    P3( X, &C, D, A, B, 11, 16, 0x6D9D6122 );
    P3( X, &B, C, D, A, 14, 23, 0xFDE5380C );
    P3( X, &A, B, C, D,  1,  4, 0xA4BEEA44 );
    P3( X, &D, A, B, C,  4, 11, 0x4BDECFA9 );
    P3( X, &C, D, A, B,  7, 16, 0xF6BB4B60 );
    P3( X, &B, C, D, A, 10, 23, 0xBEBFBC70 );
    P3( X, &A, B, C, D, 13,  4, 0x289B7EC6 );
    P3( X, &D, A, B, C,  0, 11, 0xEAA127FA );
    P3( X, &C, D, A, B,  3, 16, 0xD4EF3085 );
    P3( X, &B, C, D, A,  6, 23, 0x04881D05 );
    P3( X, &A, B, C, D,  9,  4, 0xD9D4D039 );
    P3( X, &D, A, B, C, 12, 11, 0xE6DB99E5 );
    P3( X, &C, D, A, B, 15, 16, 0x1FA27CF8 );
    P3( X, &B, C, D, A,  2, 23, 0xC4AC5665 );
    P4( X, &A, B, C, D,  0,  6, 0xF4292244 );
    P4( X, &D, A, B, C,  7, 10, 0x432AFF97 );
    P4( X, &C, D, A, B, 14, 15, 0xAB9423A7 );
    P4( X, &B, C, D, A,  5, 21, 0xFC93A039 );
    P4( X, &A, B, C, D, 12,  6, 0x655B59C3 );
    P4( X, &D, A, B, C,  3, 10, 0x8F0CCC92 );
    P4( X, &C, D, A, B, 10, 15, 0xFFEFF47D );
    P4( X, &B, C, D, A,  1, 21, 0x85845DD1 );
    P4( X, &A, B, C, D,  8,  6, 0x6FA87E4F );
    P4( X, &D, A, B, C, 15, 10, 0xFE2CE6E0 );
    P4( X, &C, D, A, B,  6, 15, 0xA3014314 );
    P4( X, &B, C, D, A, 13, 21, 0x4E0811A1 );
    P4( X, &A, B, C, D,  4,  6, 0xF7537E82 );
    P4( X, &D, A, B, C, 11, 10, 0xBD3AF235 );
    P4( X, &C, D, A, B,  2, 15, 0x2AD7D2BB );
    P4( X, &B, C, D, A,  9, 21, 0xEB86D391 );
    ctx->state[0] += A;
    ctx->state[1] += B;
    ctx->state[2] += C;
    ctx->state[3] += D; }

void md5_update( md5_context *ctx, uint8 *input, uint32 length )
  { uint32 left, fill;
    if( length < 1 ) return;
    left = ctx->total[0] & 0x3F;
    fill = 64 - left;
    ctx->total[0] += length;
    ctx->total[0] &= 0xFFFFFFFF;
    if( ctx->total[0] < length )
        ctx->total[1]++;
    if( left && length >= fill )
      { memcpy( (void *) (ctx->buffer + left),
                (void *) input, fill );
        md5_process( ctx, ctx->buffer );
        length -= fill;
        input  += fill;
        left = 0; }
    while( length >= 64 )
      { md5_process( ctx, input );
        length -= 64;
        input  += 64; }
    if( length >= 1 )
      memcpy( (void *) (ctx->buffer + left),
              (void *) input, length ); }

void md5_finish( md5_context *ctx, uint8 digest[16] )
  { static uint8 md5_padding[64] =
     { 0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    uint32 last, padn;
    uint32 high, low;
    uint8 msglen[8];
    high = ( ctx->total[0] >> 29 )
         | ( ctx->total[1] <<  3 );
    low  = ( ctx->total[0] <<  3 );
    PUT_UINT32( low,  msglen, 0 );
    PUT_UINT32( high, msglen, 4 );
    last = ctx->total[0] & 0x3F;
    padn = ( last < 56 ) ? ( 56 - last ) : ( 120 - last );
    md5_update( ctx, md5_padding, padn );
    md5_update( ctx, msglen, 8 );
    PUT_UINT32( ctx->state[0], digest,  0 );
    PUT_UINT32( ctx->state[1], digest,  4 );
    PUT_UINT32( ctx->state[2], digest,  8 );
    PUT_UINT32( ctx->state[3], digest, 12 ); }
/* --- end-of-function md5str() and "friends" --- */


#if defined(GIF)
/* ==========================================================================
 * Function:	GetPixel ( int x, int y )
 * Purpose:	callback for GIF_CompressImage() returning the
 *		pixel at column x, row y
 * --------------------------------------------------------------------------
 * Arguments:	x (I)		int containing column=0...width-1
 *				of desired pixel
 *		y (I)		int containing row=0...height-1
 *				of desired pixel
 * --------------------------------------------------------------------------
 * Returns:	( int )		0 or 1, if pixel at x,y is off or on
 * --------------------------------------------------------------------------
 * Notes:     o
 * ======================================================================= */
/* --- entry point --- */
int	GetPixel ( int x, int y )
{
int	ipixel = y*raster_width + x;	/* pixel index for x,y-coords*/
int	pixval =0;			/* value of pixel */
if ( !isaa )				/* use bitmap if not anti-aliased */
  pixval = (int)getlongbit(bitmap_raster->pixmap,ipixel); /*pixel = 0 or 1*/
else					/* else use anti-aliased grayscale*/
  pixval = (int)(colormap_raster[ipixel]); /* colors[] index number */
if ( msgfp!=NULL && msglevel>=9999 )	/* dump pixel */
  { fprintf(msgfp,"GetPixel> x=%d, y=%d  pixel=%d\n",x,y,pixval);
    fflush(msgfp); }
return pixval;
} /* --- end-of-function GetPixel() --- */
#endif /* gif */
#endif /* driver */
#endif /* PART1 */


int mystrncmpi(const char* s1, const char* s2,int n)
{
    int i=0;
    while((s1[i]!='\0' || s2[i]!='\0') && i<n) {
        if((s1[i]==s2[i])|| (s1[i]-s2[i])==32|| (s1[i]-s2[i])==- 32)
            i++;
        else
            return(s1[i]-s2[i]);
    }

    return 0;
}

const char *strcasestr(const char *s1, const char *s2)
{
 // if either pointer is null
 if (s1 == 0 || s2 == 0)
  return 0;
 // the length of the needle
 size_t n = strlen(s2);
 // iterate through the string
 while(*s1)
 // if the compare which is case insensitive is a match, return the pointer
 if(!mystrncmpi(s1++,s2,n))
  return (s1-1);
 // no match was found
 return 0;
}

/* ======================= END-OF-FILE MIMETEX.C ========================= */

