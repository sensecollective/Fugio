#ifndef DEVICEOCULUSRIFT_H
#define DEVICEOCULUSRIFT_H

#if defined( OCULUS_PLUGIN_SUPPORTED )
#include <GL/glew.h>
#endif

#include <QObject>
#include <QSettings>
#include <QDebug>
#include <QSharedPointer>

#if defined( OCULUS_PLUGIN_SUPPORTED )
#include <OVR_CAPI_GL.h>
#include <Extras/OVR_Math.h>

using namespace OVR;
#endif

class DeviceOculusRift : public QObject
{
	Q_OBJECT

public:
	static void deviceInitialise( void );
	static void deviceDeinitialise( void );
	static void devicePacketStart( qint64 pTimeStamp );
	static void devicePacketEnd( void );

	static void deviceCfgSave( QSettings &pDataStream );
	static void deviceCfgLoad( QSettings &pDataStream );

	static QSharedPointer<DeviceOculusRift> newDevice( void );

	static void delDevice( QSharedPointer<DeviceOculusRift> pDelDev );

	static QList<QWeakPointer<DeviceOculusRift> > devices( void );

public:
	explicit DeviceOculusRift( void );

	virtual ~DeviceOculusRift( void );

#if defined( OCULUS_PLUGIN_SUPPORTED )
	ovrSession hmd( void )
	{
		return( mHMD );
	}
#endif

	void drawStart( void );
	void drawEnd( void );

	void drawEyeStart( int pEyeIdx );
	void drawEyeEnd( int pEyeIdx );

#if defined( OCULUS_PLUGIN_SUPPORTED )
	ovrPosef eyeRenderPos( int pEyeIdx ) const
	{
		return( EyeRenderPose[ pEyeIdx ] );
	}

	ovrFovPort defaultEyeFOV( int pEyeIdx ) const;
#endif

private:
#if defined( OCULUS_PLUGIN_SUPPORTED )
	//---------------------------------------------------------------------------------------
	struct DepthBuffer
	{
		GLuint        texId;

		DepthBuffer(Sizei size, int sampleCount)
		{
			Q_UNUSED( sampleCount )

			Q_ASSERT(sampleCount <= 1); // The code doesn't currently handle MSAA textures.

			glGenTextures(1, &texId);
			glBindTexture(GL_TEXTURE_2D, texId);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			GLenum internalFormat = GL_DEPTH_COMPONENT24;
			GLenum type = GL_UNSIGNED_INT;
			if (GLEW_ARB_depth_buffer_float)
			{
				internalFormat = GL_DEPTH_COMPONENT32F;
				type = GL_FLOAT;
			}

			glTexImage2D( GL_TEXTURE_2D, 0, internalFormat, size.w, size.h, 0, GL_DEPTH_COMPONENT, type, NULL );

			glBindTexture (GL_TEXTURE_2D, 0 );
		}
		~DepthBuffer()
		{
			if (texId)
			{
				glDeleteTextures(1, &texId);
				texId = 0;
			}
		}
	};

	//--------------------------------------------------------------------------
	struct TextureBuffer
	{
		ovrSession          Session;
		ovrTextureSwapChain  TextureChain;
		GLuint              texId;
		GLuint              fboId;
		Sizei               texSize;

		TextureBuffer(ovrSession session, bool rendertarget, bool displayableOnHmd, Sizei size, int mipLevels, unsigned char * data, int sampleCount) :
			Session(session),
			TextureChain(nullptr),
			texId(0),
			fboId(0),
			texSize(0, 0)
		{
			Q_ASSERT(sampleCount <= 1); // The code doesn't currently handle MSAA textures.

			texSize = size;

			if (displayableOnHmd)
			{
				// This texture isn't necessarily going to be a rendertarget, but it usually is.
				Q_ASSERT(session); // No HMD? A little odd.
				Q_ASSERT(sampleCount == 1); // ovr_CreateSwapTextureSetD3D11 doesn't support MSAA.

				ovrTextureSwapChainDesc desc = {};
				desc.Type = ovrTexture_2D;
				desc.ArraySize = 1;
				desc.Width = size.w;
				desc.Height = size.h;
				desc.MipLevels = 1;
				desc.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;
				desc.SampleCount = 1;
				desc.StaticImage = ovrFalse;

				ovrResult result = ovr_CreateTextureSwapChainGL(Session, &desc, &TextureChain);

				int length = 0;
				ovr_GetTextureSwapChainLength(session, TextureChain, &length);

				if(OVR_SUCCESS(result))
				{
					for (int i = 0; i < length; ++i)
					{
						GLuint chainTexId;
						ovr_GetTextureSwapChainBufferGL(Session, TextureChain, i, &chainTexId);
						glBindTexture(GL_TEXTURE_2D, chainTexId);

						if (rendertarget)
						{
							glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
							glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
							glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
							glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
						}
						else
						{
							glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
							glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
							glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
							glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
						}
					}
				}
			}
			else
			{
				glGenTextures(1, &texId);
				glBindTexture(GL_TEXTURE_2D, texId);

				if (rendertarget)
				{
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				}
				else
				{
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				}

				glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, texSize.w, texSize.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			}

			if (mipLevels > 1)
			{
				glGenerateMipmap(GL_TEXTURE_2D);
			}

			glGenFramebuffers(1, &fboId);
		}

		~TextureBuffer()
		{
			if (TextureChain)
			{
				ovr_DestroyTextureSwapChain(Session, TextureChain);
				TextureChain = nullptr;
			}
			if (texId)
			{
				glDeleteTextures(1, &texId);
				texId = 0;
			}
			if (fboId)
			{
				glDeleteFramebuffers(1, &fboId);
				fboId = 0;
			}
		}

		Sizei GetSize() const
		{
			return texSize;
		}

		void SetAndClearRenderSurface(DepthBuffer* dbuffer)
		{
			GLuint curTexId;
			if (TextureChain)
			{
				int curIndex;
				ovr_GetTextureSwapChainCurrentIndex(Session, TextureChain, &curIndex);
				ovr_GetTextureSwapChainBufferGL(Session, TextureChain, curIndex, &curTexId);
			}
			else
			{
				curTexId = texId;
			}

			glBindFramebuffer(GL_FRAMEBUFFER, fboId);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, curTexId, 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, dbuffer->texId, 0);

			glViewport(0, 0, texSize.w, texSize.h);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glEnable(GL_FRAMEBUFFER_SRGB);
		}

		void UnsetRenderSurface()
		{
			glBindFramebuffer(GL_FRAMEBUFFER, fboId);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		void Commit()
		{
			if (TextureChain)
			{
				ovr_CommitTextureSwapChain( Session, TextureChain);
			}
		}
	};
#endif

private:
	static QList<QWeakPointer<DeviceOculusRift>>	 mDeviceList;

#if defined( OCULUS_PLUGIN_SUPPORTED )
	void globalFrameStart( qint64 pTimeStamp );

	bool deviceOpen( void );
	void deviceClose( void );

	ovrGraphicsLuid			 mLUID;
	ovrSession				 mHMD;
	ovrSessionStatus		 mSessionStatus;
	long long				 frameIndex;
	double					 sensorSampleTime;

	TextureBuffer							*eyeRenderTexture[ 2 ];
	DepthBuffer								*eyeDepthBuffer[ 2 ];
	ovrEyeRenderDesc						 EyeRenderDesc[ 2 ];
	bool									 isVisible;
	GLuint									 mirrorFBO;
	ovrMirrorTexture						 mirrorTexture;
	ovrVector3f								 ViewOffset[ 2 ];
	ovrPosef								 EyeRenderPose[ 2 ];
	GLint									 CurFBO;
#endif
};

#endif // DEVICEOCULUSRIFT_H
