/************************************************************************************************
*
*		MTC Project - Client Main Project - ClientGlobalVariables
*
*
*		@desc The global variables for the client.
*		@author igarciad
*
************************************************************************************************/
//based on www.yolinux.com/TUTORIALS/C++Singleton.html and stackoverflow.com/questions/19237931/explanation-of-c-singleton-code

#pragma once

#include <QHash>
#include <Qvariant>
#include <QVector3D>

class G{
public:

	static G& global();
	static QHash<QString,QVariant> g;

	QVariant operator [](QString i) const    {return g[i];}
	QVariant & operator [](QString i) {return g[i];}

	static QVector3D getQVector3D(QString i){
		if (!g.contains(i)){ printf("Global does not contain type %s\n", i.toUtf8().constData()); return QVector3D(); }
		return g[i].value<QVector3D>();}
	static float getFloat(QString i){
		if (!g.contains(i)){ printf("Global does not contain type %s\n", i.toUtf8().constData()); return 0; }
		return g[i].toFloat();}
	static float getDouble(QString i){
		if (!g.contains(i)){ printf("Global does not contain type %s\n", i.toUtf8().constData()); return 0; }
		return g[i].toDouble();}
	static int getInt(QString i){
		if (!g.contains(i)){ printf("Global does not contain type %s\n", i.toUtf8().constData()); return 0; }
		return g[i].toInt();}
	static bool getBool(QString i){
		if (!g.contains(i)){ printf("Global does not contain type %s\n", i.toUtf8().constData()); return false; }
		return g[i].toBool();}

private:
	G(){};  // Private so that it can  not be called
	G(G const&){};             // copy constructor is private
	G& operator=(G const&){};  // assignment operator is private
};

