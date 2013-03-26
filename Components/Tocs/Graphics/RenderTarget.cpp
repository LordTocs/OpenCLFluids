#include "RenderTarget.h"
#include "GLHeader.h"


namespace Tocs {
namespace Graphics {

RenderTarget::RenderTarget(void)
	: ID (0)
{
	glGenFramebuffers (1, (GLuint *)&ID);
}


RenderTarget::~RenderTarget(void)
{
	glDeleteFramebuffers (1, (GLuint *)&ID);
}

void RenderTarget::InternalBind ()
{
	glBindFramebuffer (GL_FRAMEBUFFER, ID);
}

void RenderTarget::Bind ()
{
	InternalBind ();
	if (BufferList.size () > 0)
	{
		glDrawBuffers (BufferList.size (),(GLenum *)&BufferList[0]);
	}
}

void RenderTarget::UnBind ()
{
	glBindFramebuffer (GL_FRAMEBUFFER, 0);
}

void RenderTarget::SetTexture (const Texture2D &texture, int output)
{
	InternalBind ();
	if (texture.Format () == TextureFormat::Depth16F || texture.Format () == TextureFormat::Depth32F)
		return; //Throw exception?
	glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + output,GL_TEXTURE_2D,texture.GetID (),0);
	BufferList.push_back (output);
	UnBind ();
}

void RenderTarget::SetDepthBuffer (const DepthStencilBuffer &texture)
{
	InternalBind ();

	glFramebufferTexture2D (GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,texture.GetID (),0);
	glFramebufferTexture2D (GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D,texture.GetID (),0);

	UnBind ();
}

}}
