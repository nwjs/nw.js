/**
 * portfinder.js typescript definitions.
 *
 * (C) 2011, Charlie Robbins
 */

type PortfinderCallback = (err: Error, port: number) => void;

interface PortFinderOptions{
  /**
   * Host to find available port on.
   */
  host?: string;
  /**
   * Minimum port (takes precedence over `basePort`).
   */
  port?: number;
}

/**
 * The lowest port to begin any port search from.
 */
export let basePort: number;

/**
 * Responds with a unbound port on the current machine.
 */
export function getPort(callback: PortfinderCallback): void;
export function getPort(options: PortFinderOptions, callback: PortfinderCallback): void;
/**
 * Responds a promise of an unbound port on the current machine.
 */
export function getPortPromise(options?: PortFinderOptions): Promise<number>;
