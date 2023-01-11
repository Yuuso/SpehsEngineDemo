#include "stdafx.h"
#include "ParticleSystem.h"

using namespace se::graphics;
using namespace se::time;


#if 0
namespace se::xpr
{
	Emitter_CpuBillboard::Emitter_CpuBillboard(DemoContext& _context)
		: ParticleEmitter(_context)
	{
		TextureModes genModes;
		genModes.sampleMin = genModes.sampleMag = genModes.sampleMip = TextureSamplingMode::Point;

		#define ColorToU8Data(COLOR) \
		uint8_t((COLOR & 0xff000000) >> 24), \
		uint8_t((COLOR & 0x00ff0000) >> 16), \
		uint8_t((COLOR & 0x0000ff00) >> 8), \
		uint8_t((COLOR & 0x000000ff) >> 0)

		TextureInput textureInput;
		textureInput.width = textureInput.height = 3;
		textureInput.data = {
			ColorToU8Data(se::Clear),	ColorToU8Data(se::Gray),	ColorToU8Data(se::Clear),
			ColorToU8Data(se::Gray),	ColorToU8Data(se::White),	ColorToU8Data(se::Gray),
			ColorToU8Data(se::Clear),	ColorToU8Data(se::Gray),	ColorToU8Data(se::Clear),
		};
		auto texture = demoContext.textureManager.create("Emitter_CpuBillboard_fallback", textureInput, genModes);

		auto shader = demoContext.shaderManager.find("tex_billboard");
		material = std::make_shared<Material>();
		material->setShader(shader, ShaderVariant::Billboard);
		material->setTexture(texture, "s_texColor", 0);
		material->setName("Emitter_CpuBillboard");
	}

	void Emitter_CpuBillboard::update(Time _dt)
	{
		if (!active || paused)
		{
			return;
		}
		if (params.duration.has_value() && emitterTime >= params.duration)
		{
			stop();
			return;
		}
		if (emitterTime < params.delay)
		{
			emitterTime += _dt;
			return;
		}

		while ((emitterTime - nextEmit) > Time::zero)
		{
			#define EMIT_BREAK nextEmit += params.frequency.resolve(); continue

			const uint32_t emitQuantity = params.quantity.resolve();
			if (particles.size() >= static_cast<size_t>(params.maxParticles))
			{
				if (params.preventEmitIfMaxParticles)
				{
					EMIT_BREAK;
				}
				removeParticles(emitQuantity);
			}

			createParticles(emitQuantity);
			EMIT_BREAK;
		}

		updateParticles();
		emitterTime += _dt;
	}
	void Emitter_CpuBillboard::start()
	{
		if (active)
		{
			return;
		}
		active = true;
		emitterTime = Time::zero;
		nextEmit = Time::zero;
		// TODO: presimulate
	}
	void Emitter_CpuBillboard::pause()
	{
		if (!active)
		{
			return;
		}
		paused = true;
	}
	void Emitter_CpuBillboard::stop()
	{
		active = false;
		paused = false;
	}
	bool Emitter_CpuBillboard::isActive() const
	{
		return active;
	}

	void Emitter_CpuBillboard::createParticles(uint32_t _amount)
	{
		auto i = particles.size() - 1;
		particles.insert(particles.end(), _amount, {});
		for (; i < particles.size(); i++)
		{
			Shape& shape = particles[i].shape;
			shape.generate(ShapeType::Square, {}, &demoContext.shapeGenerator);
			shape.setMaterial(material);
			shape.setRenderFlags(BlendAlpha | DepthTestLess | BillboardSpherical);
			demoContext.scene.add(shape);

			ParticleParams& pparams = particles[i].params;
			#define OPT_SET(VAR) if (params.VAR.has_value()) pparams.VAR

			pparams.emitTime = nextEmit;
			OPT_SET(lifetime) = params.lifetime->resolve();

			pparams.initialPosition = params.bounds->getPosition(nextEmit);
			pparams.initialVelocity = resolve(params.initialVelocity, nextEmit);
			OPT_SET(linearAcceleration) = params.linearAcceleration->resolve();
			OPT_SET(linearDrag) = params.linearDrag->resolve();
			OPT_SET(orbitTarget) = params.orbitTarget;
			pparams.orbitStrength = params.orbitStrength.resolve();
			OPT_SET(linearOscilation) = params.linearOscilation;
			pparams.linearOscilationSpeed = params.linearOscilationSpeed.resolve();
			pparams.orientationAffectsLinearOscilation = params.orientationAffectsLinearOscilation;

			// ...
			// Some sort of modules instead of this shit?
		}
	}
	void Emitter_CpuBillboard::removeParticles(uint32_t _amount)
	{
		particles.erase(particles.begin(), particles.begin() + _amount);
	}
	void Emitter_CpuBillboard::updateParticles()
	{

	}
}
#endif