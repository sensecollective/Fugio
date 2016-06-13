#ifndef FREQUENCYBANDSNODE_H
#define FREQUENCYBANDSNODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>
#include <fugio/core/variant_interface.h>

class FrequencyBandsNode : public fugio::NodeControlBase
{
	Q_OBJECT

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Generates frequency bands from a FFT" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Frequency_Bands" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE FrequencyBandsNode( QSharedPointer<fugio::NodeInterface> pNode);

	virtual ~FrequencyBandsNode( void ) {}


	// NodeControlInterface interface
public:
	virtual void inputsUpdated( qint64 pTimeStamp );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinFFT;

	QVector<QSharedPointer<fugio::PinInterface>>	 mPinOutput;
	QVector<fugio::VariantInterface *>				 mValOutput;
};


#endif // FREQUENCYBANDSNODE_H