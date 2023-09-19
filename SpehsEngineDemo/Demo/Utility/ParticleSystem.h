#pragma once

#include "glm/vec3.hpp"
#include "glm/gtc/quaternion.hpp"
#include "SpehsEngine/Core/Color.h"
#include "SpehsEngine/Core/RNG.h"
#include "SpehsEngine/Core/SE_Time.h"
#include "SpehsEngine/Graphics/InstanceBuffer.h"
#include "SpehsEngine/Graphics/Shape.h"
#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>
#include <deque>
#include <map>

#include "DemoContext.h"


namespace se::xpr
{

	//
	//	+ each has modifier for particle life time & emitter time
	//		? cross modifier per each mod
	//			- velocity, angular velocity, growth, lifetime, emitter time...
	//	+ possibility to add external params
	//		- string name, dynamic type/predefined
	//			- bool, float, vec3, quat, primitive, model...
	//		- in emitter or particle system?
	//
	// - position
	// - velocity
	//		- initial position bounds
	//			- point
	//			- shape surface/volume
	//			- model vertex
	//			+ mods over emitter time...
	//		- initial position randomizer
	//		- initial velocity randomizer
	//			- cone
	//		- acceleration
	//		- drag
	//		- attraction
	//		- orbit
	//		- position oscilator
	//
	// - orientation
	// - angular velocity
	//		- initial orientation randomizer
	//		- initial velocity randomizer
	//		- velocity acceleration
	//		- velocity drag
	//		- orientation oscilator
	//
	// - scale
	// - growth
	//		- initial scale randomizer
	//		- initial growth randomizer
	//		- growth acceleration
	//		- growth drag
	//		- scale oscilator
	//
	// - color
	//		- texture
	//			- animation?
	//		- initial color randomizer
	//		- color "blend" effect over time
	//		- color oscilator
	//
	// - life time
	//		- initial randomizer
	//		- initial life time over emitter time
	//		- kill particle conditions
	//
	// - emitter spawn rate
	//		- impulse? continuous? initial impulse only?
	//		- emitter duration
	//		- kill emitter conditions
	//

	// preSimulate ?

	// actual particle data, then reference that data
	// apply modifiers etc.
	// efficient memory gpu memory management? later, start with primitives to prove the concept
	struct ParticleData
	{
		glm::vec3 position;
		glm::quat orientation;
		glm::vec3 scale;
		glm::vec2 uv;
		Color color;
	};
	struct ParticleParams
	{
		ParticleData& data;
		glm::vec3 velocity;
		glm::quat rotation;
		glm::vec3 growth;
		time::Time lifeTime;
	};
	struct EmitterParams
	{
		time::Time spawnInterval;
		uint16_t spawnQuantity;
		std::shared_ptr<gfx::Texture> texture;
	};

	enum class ParticleModuleType : uint64_t
	{
		Unknown,

		Initial				= 1 << 0,
		OverTime			= 1 << 1,

		Position			= 1 << 2,
		Orientation			= 1 << 3,
		Scale				= 1 << 4,
		UV					= 1 << 5,
		Color				= 1 << 6,

		Velocity			= 1 << 7,
		Rotation			= 1 << 8,
		Growth				= 1 << 9,
		LifeTime			= 1 << 10,

		SpawnInterval		= 1 << 11,
		SpawnQuantity		= 1 << 12,
	};

	class ParticleMod
	{
	public:
		virtual void apply(ParticleParams&) = 0;
	private:
	};
	class EmitterMod
	{
	public:
		virtual void apply(EmitterParams&) = 0;
	private:
	};




#if 0
	class EmitterBounds
	{
	public:
		virtual glm::vec3 getPosition(se::time::Time _emitterTime) = 0;
	};

	template <typename T>
	struct RangeVariable
	{
		RangeVariable(const T& _min, const T& _max)
			: min(_min), max(_max) {}
		RangeVariable(const T& _other)
			: min(_other), max(_other) {}
		bool operator=(const T& _other)
		{
			min = _other;
			max = _other;
		}
		T resolve() const
		{
			if (min == max) return min;
			return glm::mix(min, max, se::rng::random(0.0f, 1.0f));
		}

		T min {};
		T max {};
	};

	template <typename T>
	using OptionalRangeVariable = std::optional<RangeVariable<T>>;

	template <typename T>
	using OptionalOscilation = std::optional<std::pair<T, T>>;

	template <typename T>
	using LifetimeRangeVariable = std::map<se::time::Time, RangeVariable<T>>;

	template <typename T>
	T resolve(const LifetimeRangeVariable<T>& _var, se::time::Time _time)
	{
		if (_var.size() == 0)
		{
			se_assert_m(false, "LifetimeRangeVariable should not be empty!");
			return {};
		}
		if (_var.size() == 1 && _time <= _var.begin()->first)
		{
			return _var.begin()->second.resolve();
		}
		if (_time >= _var.rbegin()->first)
		{
			return _var.rbegin()->second.resolve();
		}
		for (auto it = _var.begin(); it != _var.end(); it++)
		{
			if (_time == it->first)
			{
				return it->second.resolve();
			}
			se_assert(_time > it->first);
			auto next = std::next(it);
			se_assert(next != _var.end());
			if (_time < next->first)
			{
				auto firstDiff = it->first - _time;
				auto secDiff = next->first - it->first;
				const float factor = (float)firstDiff.value / (float)secDiff.value;
				return RangeVariable<T>{
					glm::mix(it->second.min, next->second.min, factor),
					glm::mix(it->second.max, next->second.max, factor)
				}.resolve();
			}
		}
		se_assert(false);
		return {};
	}



	//
	// base value
	// base velocity/growth
	// - ! no base values
	// - mod emitter bounds
	// - mod accelerant
	// - mod drag
	// - mod position orbit/attraction
	// - mod oscilator
	// - mod range randomizer
	// - mod over particle life time
	// - mod over emitter time
	//
	// - order matters
	// - same template for particles params?
	// - emitter can disallow mods?
	//
	//
	//
	// mod chain
	// - efficiency?
	// - randomizing?
	//	- per particle
	// - params:
	//  - emitter time
	//  - particle time
	//  ? particle instance, for randomizing
	//

	struct ModChainResolveContext
	{
		se::time::Time emitterTime;
		se::time::Time lifeTime;
		uint64_t particleId;
	};

	template <typename T>
	class EmitterModChain
	{
	public:
		typedef EmitterModChain<T> SelfType;

		virtual ~EmitterModChain()
		{
			if (next)
			{
				delete next;
			}
		}

		// Insert new mod to the chain after this
		template <typename Mod>
		requires std::derived_from<Mod, SelfType>
		Mod& insert()
		{
			Mod* mod = new Mod();
			if (next)
			{
				next->prev = mod;
				mod->next = next;
			}
			next = mod;
			mod->prev = this;
			return *mod;
		}

		// Remove this from the chain
		void remove()
		{
			if (!prev)
			{
				se_assert_m(false, "Trying to remove the root ModChain element!");
				return;
			}

			if (next)
			{
				next->prev = prev;
			}
			prev->next = next;
			delete this;
		}

		// Move up the chain (towards begin)
		void moveUp()
		{
			if (!prev)
			{
				return;
			}
			if (next)
			{
				next->prev = prev;
			}
			prev->next = next;
		}

		void resolve(T& value, const ModChainResolveContext& _context);

	protected:
		void evaluate(T& value, const ModChainResolveContext& _context);

	private:
		SelfType* prev = nullptr;
		SelfType* next = nullptr;
	};


	struct EmitterParams
	{
	};

	struct EmitterParams_
	{
		////////////////////
		// Position

		// bounding area for initial position
		std::unique_ptr<EmitterBounds> bounds;

		// initial direction over duration
		LifetimeRangeVariable<glm::vec3> initialVelocity;
		float initialVelocityCone = 0.0f;

		// acceleration
		OptionalRangeVariable<glm::vec3> linearAcceleration;
		float linearAccelerationCone = 0.0f;

		// drag
		OptionalRangeVariable<float> linearDrag;

		// orbit target
		std::optional<glm::vec3> orbitTarget;
		RangeVariable<float> orbitStrength = 1.0f;

		// additive position oscilation
		OptionalOscilation<glm::vec3> linearOscilation;
		RangeVariable<float> linearOscilationSpeed = 0.0f;
		bool orientationAffectsLinearOscilation = true;



		////////////////////
		// Rotation

		// initial rotation over duration
		LifetimeRangeVariable<glm::quat> initialOrientation
			= { {se::time::Time::zero, glm::identity<glm::quat>()} };

		// initial rotational speed
		OptionalRangeVariable<glm::quat> initialAngularVelocity;

		// angular acceleration
		OptionalRangeVariable<glm::quat> angularAcceleration;

		// angular drag
		OptionalRangeVariable<float> angularDrag;

		// multiplicative rotation oscilation
		OptionalOscilation<glm::quat> angularOscilation;
		RangeVariable<float> angularOscilationSpeed = 1.0f;



		////////////////////
		// Scale

		// initial scale over duration
		LifetimeRangeVariable<glm::vec3> initialScale
			= { {se::time::Time::zero, glm::vec3{1.0f}} };

		// initial scale growth
		OptionalRangeVariable<glm::vec3> initialGrowth;

		// scale growth acceleration
		OptionalRangeVariable<glm::vec3> growthAcceleration;

		// scale growth drag
		OptionalRangeVariable<float> growthDamping;

		// multiplicative scale oscilation
		OptionalOscilation<glm::vec3> scaleOscilation;
		RangeVariable<float> scaleOscilationSpeed = 1.0f;



		////////////////////
		// Color

		// initial color over duration
		LifetimeRangeVariable<se::Color> initialColor
			= { {se::time::Time::zero, se::Color()} };

		// multiplicative color modulation over lifetime
		LifetimeRangeVariable<se::Color> colorModulation;

		// additive color oscilation
		OptionalOscilation<se::Color> colorOscilation;
		RangeVariable<float> colorOscilationSpeed = 1.0f;



		////////////////////
		// Emitter

		// particle lifetime (infinite if null)
		OptionalRangeVariable<se::time::Time> lifetime = se::time::fromSeconds(3.0);

		// emitter duration (infinite if null)
		std::optional<se::time::Time> duration;

		// initial delay
		se::time::Time delay = se::time::Time::zero;

		// pre-simulation
		bool preSimulate = false;

		// emit frequency
		RangeVariable<se::time::Time> frequency = se::time::fromSeconds(1.0);

		// emit quantity
		RangeVariable<uint32_t> quantity = 1;

		// max num particles
		uint32_t maxParticles = std::numeric_limits<uint32_t>::max();

		// max particles behavior (default behavior: oldest particle killed, new emitted)
		bool preventEmitIfMaxParticles = false;
	};

	struct ParticleParams
	{
		se::time::Time emitTime;
		std::optional<se::time::Time> lifetime;

		////////////////////
		// Position

		glm::vec3 initialPosition;
		glm::vec3 initialVelocity;
		std::optional<glm::vec3> linearAcceleration;
		std::optional<float> linearDrag;
		std::optional<glm::vec3> orbitTarget;
		float orbitStrength;
		OptionalOscilation<glm::vec3> linearOscilation;
		float linearOscilationSpeed;
		bool orientationAffectsLinearOscilation;

		////////////////////
		// Rotation

		glm::quat initialOrientation;
		std::optional<glm::quat> initialAngularVelocity;
		std::optional<glm::quat> angularAcceleration;
		std::optional<float> angularDrag;
		OptionalOscilation<glm::quat> angularOscilation;
		float angularOscilationSpeed;

		////////////////////
		// Scale

		glm::vec3 initialScale;
		std::optional<glm::vec3> initialGrowth;
		std::optional<glm::vec3> growthAcceleration;
		std::optional<float> growthDamping;
		OptionalOscilation<glm::vec3> scaleOscilation;
		float scaleOscilationSpeed = 1.0f;

		////////////////////
		// Color

		se::Color initialColor;
		std::map<se::time::Time, se::Color> colorModulation;
		OptionalOscilation<se::Color> colorOscilation;
		float colorOscilationSpeed = 1.0f;
	};

	class ParticleEmitter
	{
	public:

		ParticleEmitter(DemoContext& _context)
			: demoContext(_context) {}

		virtual void update(se::time::Time _dt) = 0;
		virtual void start() = 0;
		virtual void pause() = 0;
		virtual void stop() = 0;
		virtual void setVisibile(bool _value) = 0;
		virtual bool isActive() const = 0;

		const EmitterParams& getParams() const { return params; }
		EmitterParams& getParams() { return params; }

	protected:

		void validateParams();

		DemoContext& demoContext;
		EmitterParams params;
	};


	//
	// Billboard_Emitter_AutoInstance_GpuSimulated
	// Billboard_Emitter_AutoInstance_UniformData
	// Billboard_Emitter_AutoInstance_TextureData
	// Billboard_Emitter_InstanceBuffer_GpuSimlated
	// Billboard_Emitter_InstanceBuffer_UniformData
	// Billboard_Emitter_InstanceBuffer_TextureData
	// Billboard_Emitter_CpuSimulated
	// Billboard_Emitter_CpuSimulated_Batched
	// Primitive_Emitter_
	// Model_Emitter_
	// Line_Emitter_
	// Ribbon_Emitter_
	// Light_Emitter_
	//

	class Emitter_CpuBillboard : public ParticleEmitter
	{
	public:

		Emitter_CpuBillboard(DemoContext& _context);

		// texture
		// texture offset (atlas animation)
		// set rendermode (batched)

		virtual void update(se::time::Time _dt) override;
		virtual void start() override;
		virtual void pause() override;
		virtual void stop() override;
		virtual void setVisibile(bool _value) override;
		virtual bool isActive() const override;

	private:

		struct Particle
		{
			ParticleParams params;
			se::gfx::Shape shape;
		};

		void createParticles(uint32_t _amount);
		void removeParticles(uint32_t _amount);
		void updateParticles();

		se::time::Time emitterTime;
		se::time::Time nextEmit;
		bool active = false;
		bool paused = false;

		std::shared_ptr<se::gfx::Material> material;
		std::deque<Particle> particles;
	};


	class ParticleSystem
	{
	public:

		template <typename T>
		requires std::derived_from<T, ParticleEmitter>
		T& addEmitter(const std::string& _name);

		ParticleEmitter& getEmitter(const std::string& _name);
		ParticleEmitter& getEmitter(size_t _index);
		size_t numEmitters() const;

		void update(se::time::Time _dt);
		void start();
		void pause();
		void stop();
		bool isAnyEmitterActive() const;

	private:

		std::unordered_map<std::string, std::unique_ptr<ParticleEmitter>> emitters;
	};
#endif
}
