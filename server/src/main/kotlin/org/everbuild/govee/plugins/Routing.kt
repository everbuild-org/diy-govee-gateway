package org.everbuild.govee.plugins

import io.ktor.server.application.*
import io.ktor.server.request.*
import io.ktor.server.response.*
import io.ktor.server.routing.*
import kotlinx.serialization.Serializable

@Serializable
data class Payload (val id: String, val temperature: Float, val humidity: Float, val battery: Float)

fun Application.configureRouting() {
    routing {
        put("/api") {
            val payload = call.receive<Payload>()
            appMicrometerRegistry.gauge("${payload.id}_temperature", payload.temperature)
            appMicrometerRegistry.gauge("${payload.id}_humidity", payload.humidity)
            appMicrometerRegistry.gauge("${payload.id}_battery", payload.battery)
        }
    }
}
